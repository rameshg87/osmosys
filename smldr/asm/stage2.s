/********************************************************************************

PART OF OSMOSYS OPERATING SYSTEM; CODE WRITTEN BY TEAM OSMOSYS
v1.0

********************************************************************************/
/* smldr stage 2 */

.code16
.section .text
.globl _start
_start:
	xor %ax,%ax
	mov %ax,%ds
	mov 0x7E1C,%ax		/*move volume start lba from VBR*/
	mov 0x7E1E,%bx
	mov 0x7E0E,%cx		/*move no. of reserved sectors*/
	add %cx,%ax
	jnc next1
	inc %bx
next1:
	mov %ax,0x7E38		/*move cluster_start_lba + res_sectors*/
	mov %bx,0x7E3A
	movb 0x7E10,%cl		/*move no. of FAT's*/
	mov 0x7E24,%ax		/*move no. of sectors per FAT*/
	mov 0x7E26,%bx
loop1:
	cmpb $1,%cl		/*perform no. of FAT's * sectors per FAT*/
	jz next3
	add 0x7E24,%ax
        adc 0x7E26,%bx
	decb %cl
	jmp loop1
next3:
	movb %dl,drv_index
	add 0x7E38,%ax
	adc 0x7E3A,%bx
	mov %ax,0x7E34
	mov %bx,0x7E36	
	mov 0x7E2C,%cx 
	mov 0x7E2E,%dx
        
loop20:	
	mov $0x8000,%ax	
	call ret_clstr
	call ret_file_clstr

	cmpb $0x1,flag
	je next30
	call lnext_clstr
	jmp loop20
next30:
	mov %ax,0x7E2C
	mov %bx,0x7E2E
	mov %ax,%cx
	mov %bx,%dx
	mov $0x500,%ax
	mov %ax,%di
 	
loop40:		
	call ret_clstr
	call lnext_clstr
	xor %bh,%bh 
	movb 0x7E0D,%bl
	mov $512,%ax
	mul %bx
	add %di,%ax
	mov %ax,%di
	jmp loop40

loop30:	
	jmp 0x500	

name:	.asciz	"SMLDR ABSENT$"
 
flag: .byte 0x0

drv_index: .byte 0x0

dap:
	.byte	0x10
	.byte	0x0
	.byte	0x0
	.byte	0x0
	.2byte	0x0000
	.2byte	0x0000
	.8byte 	0x0		

	
	
ret_clstr:
	mov %ax,dap+4
	cmp $2,%ecx
	jl next4
	sub $2,%cx
	jmp next5
next4:
	sub $2,%cx
	dec %dx
next5:	
	mov %cx,%bx
        mov %dx,%si
	movb 0x7E0D,%al
	movb %al,dap+2
next6:	
	cmpb $1,%al
	je next8
	add %bx,%cx
	adc %si,%dx
	decb %al
	jmp next6
next8:
	mov 0x7E34,%ax
	mov 0x7E36,%bx
	add %cx,%ax
	adc %dx,%bx
given_lba:	
	mov %ax,dap+8	
	mov %bx,dap+10
	movb $0x42,%ah
	movb drv_index,%dl 
	lea dap,%si
	int $0x13
	ret
	
	
ret_file_clstr:
	mov $0x8020,%bx
	xor %ch,%ch
	movb 0x7E0D,%cl
	shl $0x4,%cx
	/*mov $8,%ax
	mul %cx
	mov %ax,%cx*/		
loop10:
	mov $5,%dx
	mov %bx,%si
	lea name,%di
loop11:
	movb (%di),%al
	cmpb %al,(%si)
	jne next10
	inc %si
	inc %di
	dec %dx
	jnz loop11
	cmpb $0x20,(%si)
	je next11
next10:	
	add $32,%bx
	dec %cx
	jnz loop10
	jmp return
next11:
	add $15,%si
	mov (%si),%bx
	add $6,%si
	mov (%si),%ax
	movb $0x01,flag
return:	
	ret

lnext_clstr:
	mov 0x7E2C,%ax 
	mov 0x7E2E,%bx
        shr $0x7,%ax
	mov %bx,%cx
	shl $0x9,%cx
	or %cx,%ax
	shr $0x7,%bx
	add 0x7E38,%ax
	adc 0x7E3A,%bx
	mov $0x9000,%cx
	mov %cx,dap+4
	movb $0x1,%cl
	movb %cl,dap+2
	call given_lba
	mov 0x7E2C,%bx
	and $0x007F,%bx
	shl $0x2,%bx
	/*mov $0x4,%ax
	mul %bx
	mov %ax,%bx*/
	add $0x9000,%bx
	mov %cs:(%bx),%cx
	add $2,%bx
	mov %cs:(%bx),%dx
        and $0x0FFF,%dx
	cmpw $0x0FFF,%dx
	jne next20
        cmpw $0xFFF8,%cx
	jl next20
	cmpb $0x1,flag
	je loop30
next40:
	lea name,%si
        call 0x7C9A

idle:	jmp idle

next20:
	mov %cx,0x7E2C
	mov %dx,0x7E2E
	ret

