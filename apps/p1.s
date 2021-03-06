.code32

.globl _start
.globl print
.globl putc
.globl gety
.globl setattrib
.globl getc
.globl geti
.globl gets
.globl gotoxy
.globl clrscr
.globl malloc
.globl free
.globl open
.globl read
.globl write
.globl close
.globl create_process
.globl kill_process
.globl cd
.globl ls
.globl file_search
.globl strlen
.globl strcpy
.globl strcat
.globl shutdown
.globl reboot
.globl strcmp
.globl mkfile
.globl mkdir
.globl rm
.globl sysinfo 

.section .text

_start:
	call pmain
	call ret_pid
	push %eax
	call kill_process
loop:	jmp loop

print:
	pop %ecx
	mov $0x1,%eax
	mov $0x2,%ebx
	int $99
	push %ecx
	ret

putc:
	pop %ecx
	mov $0x1,%eax
	mov $0x1,%ebx
	int $99
	push %ecx
	ret

getc:
	mov $0x2,%eax
	mov $0x0,%ebx
	int $99
	ret

geti:
	mov $0x2,%eax
	mov $0x1,%ebx
	int $99
	ret
	
gets:
	mov $0x2,%eax
	mov $0x2,%ebx	
	int $99
	ret

gotoxy:
	pop %ecx
	mov $0x1,%eax
	mov $0x3,%ebx
	int $99
	push %ecx
	ret

clrscr:
	mov $0x1,%eax
	mov $0x4,%ebx
	int $99
	ret

gety:
	pop %ecx
	mov $0x1,%eax
	mov $0x5,%ebx
	int $99
	push %ecx
	ret

setattrib:
	pop %ecx
	mov $0x1,%eax
	mov $0x6,%ebx
	int $99
	push %ecx
	ret

malloc:
	pop %ecx
	mov $0x3,%eax
	mov $0x1,%ebx
	int $99 	
	push %ecx
	ret

free:
	pop %ecx
	mov $0x3,%eax
	mov $0x2,%ebx
	int $99
	push %ecx
	ret

open:
	pop %ecx
	mov $0x4,%eax
	mov $0x1,%ebx
	int $99
	push %ecx
	ret

read:
	pop %ecx
	mov $0x4,%eax
	mov $0x2,%ebx
	int $99
	push %ecx
	ret

write:
	pop %ecx
	mov $0x4,%eax
	mov $0x3,%ebx
	int $99
	push %ecx
	ret

close:
	pop %ecx
	mov $0x4,%eax
	mov $0x4,%ebx
	int $99
	push %ecx
	ret

seek:
	pop %ecx
	mov $0x4,%eax
	mov $0x5,%ebx
	int $99
	push %ecx
	ret

create_process:
	pop %ecx
	mov $0x5,%eax
	mov $0x2,%ebx
	int $99
	push %ecx
	ret

kill_process:
	pop %ecx
	mov $0x5,%eax
	mov $0x3,%ebx
	int $99
	push %ecx
	ret

ret_pid:
	pop %ecx
	mov $0x5,%eax
	mov $0x4,%ebx
	int $99
	push %ecx
	ret

cd:
	pop %ecx
	mov $0x4,%eax
	mov $10,%ebx
	int $99
	push %ecx
	ret

ls:
	pop %ecx
	mov $0x4,%eax
	mov $9,%ebx
	int $99
	push %ecx
	ret

file_search:
	pop %ecx
	mov $0x4,%eax
	mov $11,%ebx
	int $99
	push %ecx
	ret

strlen:
	pop %ecx
	mov $0x6,%eax
	mov $0x1,%ebx
	int $99
	push %ecx
	ret

strcpy:
	pop %ecx
	mov $0x6,%eax
	mov $0x2,%ebx
	int $99
	push %ecx
	ret

strcat:
	pop %ecx
	mov $0x6,%eax
	mov $0x3,%ebx
	int $99
	push %ecx
	ret

shutdown:
	pop %ecx
	mov $0x7,%eax
	mov $0x1,%ebx
	int $99
	push %ecx
	ret

reboot:
	pop %ecx
	mov $0x7,%eax
	mov $0x2,%ebx
	int $99
	push %ecx
	ret

strcmp:
	pop %ecx
	mov $0x6,%eax
	mov $0x4,%ebx
	int $99
	push %ecx
	ret

mkfile:
	pop %ecx
	mov $0x4,%eax
	mov $0x6,%ebx
	int $99
	push %ecx
	ret

mkdir:
	pop %ecx
	mov $0x4,%eax
	mov $0x8,%ebx
	int $99
	push %ecx
	ret

rm:
	pop %ecx
	mov $0x4,%eax
	mov $0x7,%ebx
	int $99
	push %ecx
	ret

sysinfo:
	pop %ecx
	mov $0x7,%eax
	mov $0x3,%ebx
	int $99
	push %ecx
	ret

