.code32

.section .data

string1: .asciz "\nthis is our %dnd process\n"

.section .text

.globl _start

_start:
	mov $2,%eax
	push %eax
	lea string1,%ebx
     	push %ebx
	movl $0x1,%eax
	int $99
	pop %eax
	pop %eax
	pop %eax
	pop %eax
new:jmp new	
