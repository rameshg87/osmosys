/********************************************************************************

PART OF OSMOSYS OPERATING SYSTEM; CODE WRITTEN BY TEAM OSMOSYS
v1.0

********************************************************************************/
/* smldr stage 1 */

.code16
	
.section .data

pnumber:	
	.byte	0x0
serror1:
	.asciz	"DRIVE NOT BOOTABLE$"
serror2:
	.asciz	"CANNOT READ FROM SOURCE DISK$"
success:
	.asciz	"SUCCESS$"
dap:
	.byte	0x10
	.byte	0x00
	.byte	0x01
	.byte	0x00
	.4byte	0x00007E00
lba0700:
	.byte	0x00
lba1508:
	.byte	0x00
lba2316:
	.byte	0x00
lba3124:
	.byte	0x0
	.4byte	0x0

.section .text

.globl _start	



_start:	

	call clearscreen
	
	xor %ax,%ax
	mov %ax,%ds
	mov %ax,%es
	mov %ax,%ss

	mov $0x6000,%sp
	push %dx

	movb pnumber,%al

	mov $0x10,%dx
	mul %dx

	add $0x7DBE,%ax
	mov %ax,%si

	movb (%si),%al
	cmp $0x80,%al

	jne error1

	mov $0x0,%cx

	movb 0x8(%si),%cl
	movb %cl,lba0700

	movb 0x9(%si),%cl
	movb %cl,lba1508

	movb 0xa(%si),%cl
	movb %cl,lba2316

	movb 0xb(%si),%cl
	movb %cl,lba3124

	lea dap,%si
	pop %dx

	mov $0x42,%ah

	int $0x13

	jc error2

	jmp 0x7E00


error1:
	/* drive not bootable */


	lea serror1,%si
	call writestring	

loop1:
	jmp loop1

error2:	
	/* cannot read from drive */
	lea serror2,%si
	call writestring

loop2:
	jmp loop2
	

clearscreen:
	mov $0xb800,%ax
	mov %ax,%es	
	mov $0x0,%si

loop6:
	movb $' ',%es:(%si)
	inc %si
	movb $0x0f,%es:(%si)
	inc %si
	cmp $0xFA0,%si
	jne loop6
	ret
	
writestring:

	mov $0xb800,%ax
	mov %ax,%es
	mov $0x0,%di
	
loop3:
	movb (%si),%bl
	movb %bl,%es:(%di)
	inc %di
	movb $0x0F,%es:(%di)
	inc %di
	inc %si
	cmp $'$',(%si)
	jnz loop3

	ret

print_ax:
	push %bx
	push %cx
	push %dx

	mov %ds,%bx
	mov $0xb800,%cx
	mov %cx,%ds
	mov %ax,%dx

        shr $12,%ax
        and $0xF,%ax
        cmpb $0x09,%al
        jg j2
        add $48,%al
	jmp j1
j2:	add $55,%al
j1:	movb %al,0
	movb $0x1e,1

	mov %dx,%ax	
        shr $8,%ax
        and $0xF,%ax
        cmpb $0x09,%al
        jg j4
        add $48,%al
	jmp j3
j4:	add $55,%al
j3:	movb %al,2
	movb $0x1e,3

	mov %dx,%ax	
        shr $4,%ax
        and $0xF,%ax
        cmpb $0x09,%al
        jg j6
        add $48,%al
	jmp j5
j6:	add $55,%al
j5:	movb %al,4
	movb $0x1e,5

	mov %dx,%ax	
        and $0xF,%ax
        cmpb $0x09,%al
        jg j8
        add $48,%al
	jmp j7
j8:	add $55,%al
j7:	movb %al,6
	movb $0x1e,7
	mov %bx,%ds
	mov %dx,%ax	
	pop %bx
	pop %cx
	pop %dx
	ret	
	
