.code32

.section .data

string1: .asciz "\nHello %s %d %c\n"
string2: .asciz "Osmosys ver"

.section .text

.globl _start

_start:
	mov $98,%eax
	push %eax
	mov $1,%eax
	push %eax
	lea string2,%ebx
	push %ebx
	lea string1,%ebx
     	push %ebx
	movl $0x1,%eax
	int $99
	pop %eax
	pop %eax
	pop %eax
	pop %eax
new:jmp new	
