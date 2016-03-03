.code32

.globl _start

.section .data

 string1: .asciz "Enter the string"
 string2: .asciz "string entered is %s"

.section .text
_start:
	lea string1,%ebx
	push %ebx
	mov $1,%eax
	int $99
	mov $2,%eax
	mov $2,%ebx
	int $99
	push %eax
	lea string2,%ebx
	push %ebx
	mov $1,%eax
	int $99

loops:	jmp loops
