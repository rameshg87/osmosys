/********************************************************************************

PART OF OSMOSYS OPERATING SYSTEM; CODE WRITTEN BY TEAM OSMOSYS
v1.0

********************************************************************************/
/* smldr stage 3 */

/* we are still in 16-bit real mode */
.code16

.section .text

/* this is the start function of the kernel */
.globl _start
_start:
/* disable interrupts at start of kernel */
cli


	mov $0xe801,%ax
	int $0x15
	jc cannot_get_memory_size

	shr $10,%ax
	and $0x003F,%ax
	add $1,%ax
	
	shr $4,%bx
	and $0xFFF,%bx
	add %bx,%ax

	mov %ax,0xA000

	mov $0x0,%bx
	mov $0x0,%ax
	jmp got_memory_size

cannot_get_memory_size:
	mov $0xFFFF,%dx
	mov %dx,0xA000
	
got_memory_size:	


/* clear ds register */
xor	%ax,%ax
mov	%ax,%ds	

/* load the gdt with the structure */
DATA32	ADDR32	lgdt	gdtstructure

/* make the last bit of cr0 to enter the protected mode */
movl	%cr0,%eax
orl	$0x01,%eax
movl	%eax,%cr0

/* make a long jump to clear prefetch queue and enter 32-bit mode */
DATA32	ljmp	$0x8,$PMODE

/* finally in 32-bit mode */
.code32


PMODE:

/* fill in values for ds,es,fs,gs and ss registers */
	mov $0x10,%ax
	mov %ax,%ds
	mov %ax,%es
	mov %ax,%fs
	mov %ax,%gs
	mov %ax,%ss
        
/* load the stack pointer with address of bottom of system stack */
	movl $0x9FFFF,%esp

/* to enable the A20 address line inorder to address more than 1MB of memory */
        call    empty_8042
        movb    $0xd1,%al
        out     %al,$0x64
        call    empty_8042
        movb    $0xdf,%al
        out     %al,$0x60
        call    empty_8042
	jmp	A20ENABLED


/* this function repeatedly checks the keyboard controller 8042 */
empty_8042:
	.word	0x00eb,0x00eb
        in      $0x64,%al
        test    $0x2,%al
        jnz     empty_8042
        ret


/* finally we have enabled A20 address line */
A20ENABLED:
	/* call the kernel main function */

	.extern _Z5kmainv
	call _Z5kmainv

	/* sit in the idle loop if main function returns */
	.globl idle
	idle:
		jmp idle

	nop

/************************************************************************


 THE EXECUTION OF KERNEL STOPS HERE...OTHERS FUNCTIONS TO BE CALLED FROM
   KERNELS MAIN FUNCTION OR FROM OTHER FILES FOLLOW FROM HERE */
	

/* function that loads the idt - to be called later */
	.globl	loadidt
	.globl	timerhandler
	.globl	keyboardhandler
	.globl jmp_to_7C00
	.globl start_osmosys
	.globl memcpy_kernel
	.extern _Z18nexception_handlerv
	.extern _Z8keyboardv
	.extern putint_hexadecimal

start_osmosys:
	jmp 0x00100000
	ret

loadidt:
	lidt	idtstructure
	ret


memcpy_kernel:


    push %ebp
    mov %esp,%ebp
    mov 0x08(%ebp),%esi
    mov 0x0c(%ebp),%edi
    mov $512,%cx
    mov $0x20,%ax
    mov %ax,%es

mkloop:
    mov (%esi),%eax  
    mov %eax,%es:(%edi)
    add $4,%esi
    add $4,%edi
    sub $4,%cx
    jnz mkloop

    pop %ebp
    ret


jmp_to_7C00:
    cli

    mov $0x30,%ax
    mov %ax,%ds

    mov %ax,%es
    mov %ax,%fs
    mov %ax,%gs
    mov $0x8000,%esp
    mov %ax,%ss

    jmp $0x28,$on_the_way_to_real_mode    

.code16
on_the_way_to_real_mode:

    movl %cr0,%eax
    andl $0xfffffffe,%eax
    movl %eax,%cr0

    ljmp $0,$realmode

/* we are in real mode now */
realmode:

    xor %ax,%ax
    mov %ax,%ds
    mov %ax,%es
    mov %ax,%fs
    mov %ax,%gs
    mov %ax,%ss
    lidt rmidt
    sti 

    mov $0x80,%dl

    ljmp $0,$0x7C00
    
    .code32

rmidt:
	.2byte	0x3FF	/* limit of the idt */
	.4byte	0x0	/* base address of idt */

/************************************************************************

 THE FUNCTION PART ENDS HERE...DATA PART OF THE KERNEL FOLLOWS */


	.extern idt_entries
	.globl	idtstructure
idtstructure:
	.2byte	0x7FF	/* limit of the idt */
	.4byte	idt_entries	/* base address of idt */


/* gdt structure that is to be shown in the lgdt instruction */
gdtstructure:
	.2byte	0x37		/* limit of the gdt */
	.4byte	gdtstart	/* base address of gdt */

gdtstart:

	/* null descriptor to ensure that no segment registers get loaded with value 0 */
	/* causes "GENERAL PROTECTION FAULT" if we try to access this gdt entry */
	.4byte	0
	.4byte	0

	/* code segment for our bootloader */
	.2byte	0xFFFF, 0
	.byte	0, 0x9A, 0xCF, 0

	/* data segment for our bootloader */
	.2byte	0xFFFF, 0
	.byte	0, 0x92, 0xCF, 0	

	/* code segment for our kernel */
	.2byte	0xFFFF, 0
	.byte	0x10, 0x9A, 0xCF, 0

	/* data segment for our kernel */
	.2byte	0xFFFF, 0x0000
	.byte	0x10, 0x92, 0xCF, 0

	/* pseudo code segment for our chainloader */
	.2byte	0xFFFF, 0
	.byte	0x0, 0x9E, 0x0, 0

	/* psuedo data segment for our chainloader  */
	.2byte	0xFFFF, 0x0000
	.byte	0x0, 0x92, 0x0, 0

gdtend:


.extern common
.extern timer_handler
.extern keyboard
.globl commonhandler

timerhandler:

	/* disable any further interrupts */
	cli

	push $0
	push $31

	/* backup the contents of registers */

	pusha

	push %ds
	push %es
	push %fs
	push %gs

	push %esp

	mov $0x10,%ax
	
	mov %ax,%ds
	mov %ax,%es
	mov %ax,%fs
	mov %ax,%gs

	call timer_handler

	/* restore the contents of registers */

	

	pop %eax
	pop %ds
	pop %es
	pop %fs
	pop %gs
	popa	

	add $8,%esp

	iret

keyboardhandler:

	/* disable any further interrupts */
	cli

	push $0
	push $31

	/* backup the contents of registers */

	pusha

	push %ds
	push %es
	push %fs
	push %gs

	push %esp

	mov $0x10,%ax
	
	mov %ax,%ds
	mov %ax,%es
	mov %ax,%fs
	mov %ax,%gs

	call keyboard

	/* restore the contents of registers */

	

	pop %eax
	pop %ds
	pop %es
	pop %fs
	pop %gs
	popa	

	add $8,%esp

	iret


commonhandler:

	/* disable any further interrupts */
	cli

	push $0
	push $31

	/* backup the contents of registers */

	pusha

	push %ds
	push %es
	push %fs
	push %gs

	push %esp

	mov $0x10,%ax
	
	mov %ax,%ds
	mov %ax,%es
	mov %ax,%fs
	mov %ax,%gs

	call common

	/* restore the contents of registers */

	

	pop %eax
	pop %ds
	pop %es
	pop %fs
	pop %gs
	popa	

	add $8,%esp

	iret


/* block section code (bss) where uninitialized variables and arrays are stored */
.section .bss
