/********************************************************************************

PART OF OSMOSYS OPERATING SYSTEM; CODE WRITTEN BY TEAM OSMOSYS
Version 1.3.0
Last updated - 18/01/09

********************************************************************************/


.code32

.section .text 

.globl _start
.globl load_gdt
.globl load_idt
.globl load_tss
.globl timerisr
.globl load_ptbr
.globl enable_paging
.globl keyboardisr
.globl primarydiskisr
.globl secondarydiskisr
.globl syscallisr
.globl floppyisr
.globl int99
.globl load_ldt
.globl restore_process_values
.globl jmp_2_new_process
.globl exception0
.globl exception1
.globl exception2
.globl exception3
.globl exception4
.globl exception5
.globl exception6
.globl exception7
.globl exception8
.globl exception9
.globl exception10
.globl exception11
.globl exception12
.globl exception13
.globl exception14
.globl exception15
.globl exception16
.globl exception17
.globl exception18
.globl exception19
.globl exception20
.globl exception21
.globl exception22
.globl exception23
.globl exception24
.globl exception25
.globl exception26
.globl exception27
.globl exception28
.globl exception29
.globl exception30
.globl exception31
.globl int99
.globl systemcallbackup
.globl temp

.extern newswitch
.extern kmain
.extern timerhandler
.extern keyboardhandler
.extern primarydiskhandler
.extern secondarydiskhandler
.extern exception_handler
.extern syscall_handler
.extern floppy_interrupt
_start:
	
      call kmain

load_gdt:
	
	
      push %ebp
      mov %esp,%ebp
      mov 0x08(%ebp),%eax
      lgdt (%eax)
      
      ljmp $0x8,$new_gdt
      
new_gdt:

	mov $0x10,%ax
	mov %ax,%ds
	mov %ax,%es
	mov %ax,%fs
	mov %ax,%gs
	mov %ax,%ss
	
      pop %ebp
      ret 

load_idt:
      cli
      push %ebp
      mov %esp,%ebp
      mov 0x08(%ebp),%eax
      lidt (%eax)
      sti
	pop %ebp
      ret

enable_paging:
      mov %cr0,%eax
      or $0x80000000,%eax
      mov %eax,%cr0      
      jmp $0x8,$paging_enabled	

paging_enabled:
      ret

load_ptbr:
      push %ebp
      mov %esp,%ebp
      mov 0x08(%ebp),%eax
      mov %eax,%cr3
      pop %ebp
      ret

timerisr:

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

	call timerhandler

	/* restore the contents of registers */

	

	pop %eax
	pop %ds
	pop %es
	pop %fs
	pop %gs
	popa	

	add $8,%esp

	iret

restore_process_values:
      
	pop %eax

	mov temp,%esp

        pop %ds
	pop %es
	pop %fs
	pop %gs
	popa	

	add $8,%esp
      
      iret
	
load_tss:

	mov $0x2b,%ax
	ltr %ax
	ret
	

jmp_2_new_process:

	pop %eax
	xor %eax,%eax
	
	mov $0x98000,%esp

	mov $0x23,%ax
	mov %ax,%ds
	mov %ax,%es
	mov %ax,%fs
	mov %ax,%gs

	mov $0x33,%eax
	pushl %eax

	mov $0xffffffff,%eax
	pushl %eax

	pushf
	pop %eax
	or $0x200,%eax
	pushl %eax

	mov $0x1b,%eax
	pushl %eax

	mov $0x2000000,%eax
	pushl %eax

	iret

keyboardisr:
	/* disable any further interrupts */
	cli

	/* backup the contents of registers */
	push $0
	push $0
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
	mov %ax,%ss

	call keyboardhandler

	pop %eax	
	pop %gs
	pop %fs
	pop %es
	pop %ds
	popa
	add $8,%esp
	iret

primarydiskisr:
	/* disable any further interrupts */
	cli

	/* backup the contents of registers */
	push %eax
	push %ebx
	push %ecx
	push %edx
	push %esi
	push %edi
	push %ebp
	push %esp

	call primarydiskhandler

	/* restore the contents of registers */
	pop %esp
	pop %ebp
	pop %edi
	pop %esi
	pop %edx
	pop %ecx
	pop %ebx
	pop %eax
	iret

secondarydiskisr:
	/* disable any further interrupts */
	cli

	/* backup the contents of registers */
	push %eax
	push %ebx
	push %ecx
	push %edx
	push %esi
	push %edi
	push %ebp
	push %esp

	call secondarydiskhandler

	/* restore the contents of registers */
	pop %esp
	pop %ebp
	pop %edi
	pop %esi
	pop %edx
	pop %ecx
	pop %ebx
	pop %eax
	iret


floppyisr:
	/* disable any further interrupts */
	cli

	/* backup the contents of registers */
	push %eax
	push %ebx
	push %ecx
	push %edx
	push %esi
	push %edi
	push %ebp
	push %esp

	call floppy_interrupt

	/* restore the contents of registers */
	pop %esp
	pop %ebp
	pop %edi
	pop %esi
	pop %edx
	pop %ecx
	pop %ebx
	pop %eax
	iret

syscallisr:
	/* disable any further interrupts */
	cli

	/* backup the contents of registers */
	push $0
	push $0
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
	mov %ax,%ss

	call syscall_handler

	pop %eax	
	pop %gs
	pop %fs
	pop %es
	pop %ds
	popa
	add $8,%esp
	iret

exception0:
	cli
	push $0
	push $0
	jmp exceptionhandler

exception1:
	cli
	push $0
	push $1
	jmp exceptionhandler

exception2:
	cli
	push $0
	push $2
	jmp exceptionhandler

exception3:
	cli
	push $0
	push $3
	jmp exceptionhandler

exception4:
	cli
	push $0
	push $4
	jmp exceptionhandler

exception5:
	cli
	push $0
	push $5
	jmp exceptionhandler

exception6:
	cli
	push $6
	push $0
	jmp exceptionhandler

exception7:
	cli
	push $0
	push $7
	jmp exceptionhandler

exception8:
	cli
	push $8
	jmp exceptionhandler

exception9:
	cli
	push $0
	push $9
	jmp exceptionhandler

exception10:
	cli
	push $10
	jmp exceptionhandler

exception11:
	cli
	push $11
	jmp exceptionhandler

exception12:
	cli
	push $12
	jmp exceptionhandler

exception13:
	cli
	push $13
	jmp exceptionhandler

exception14:
	cli
	push $14
	jmp exceptionhandler

exception15:
	cli
	push $0
	push $15
	jmp exceptionhandler

exception16:
	cli
	push $0
	push $16
	jmp exceptionhandler

exception17:
	cli
	push $0
	push $17
	jmp exceptionhandler

exception18:
	cli
	push $0
	push $18
	jmp exceptionhandler

exception19:
	cli
	push $0
	push $19
	jmp exceptionhandler

exception20:
	cli
	push $0
	push $20
	jmp exceptionhandler

exception21:
	cli
	push $0
	push $21
	jmp exceptionhandler

exception22:
	cli
	push $0
	push $22
	jmp exceptionhandler

exception23:
	cli
	push $0
	push $23
	jmp exceptionhandler

exception24:
	cli
	push $0
	push $24
	jmp exceptionhandler

exception25:
	cli
	push $0
	push $25
	jmp exceptionhandler

exception26:
	cli
	push $0
	push $26
	jmp exceptionhandler

exception27:
	cli
	push $0
	push $27
	jmp exceptionhandler

exception28:
	cli
	push $0
	push $28
	jmp exceptionhandler

exception29:
	cli
	push $0
	push $29
	jmp exceptionhandler

exception30:
	cli
	push $0
	push $30
	jmp exceptionhandler

exception31:
	cli
	push $0
	push $31
	jmp exceptionhandler


exceptionhandler:

	/* backup the contents of registers */
	pusha
	push %ds
	push %es
	push %fs
	push %gs
	push %esp
	mov $0x10,%eax
	mov %eax,%ds
	mov %eax,%es
	mov %eax,%fs
	mov %eax,%gs
	
	call exception_handler

	/* restore the contents of registers */
	pop %eax
	pop %gs
	pop %fs
	pop %es
	pop %ds
	popa
	add $8,%esp
	iret

systemcallbackup:


	pop %ebx

	/* push restore values */
	pushl %ss
	pushl %esp
	add $8,%esp
	pushf
	pushl %cs
	pushl %ebx

	/* dummy codes */
	push $0
	push $0

	pusha

	push %ds
	push %es
	push %fs
	push %gs

	push %esp
	call newswitch
	


