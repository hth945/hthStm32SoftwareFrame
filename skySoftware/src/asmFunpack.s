
    AREA |.text|, CODE, READONLY, ALIGN=2
    THUMB
    REQUIRE8
    PRESERVE8

;; 加法 a  b
;asm_add    PROC
			;EXPORT  asm_add             [WEAK]
	;MOV r2,r0
	;ADDS r0,r2,r1
	;BX      R0
			;ENDP
				
;; 数组拷贝  *data1  *data2  *len				
;asm_copy    PROC
	;EXPORT  asm_copy
	;PUSH {r4,lr}
	;MOVS r3,#0x00  
	;B ENDLOOP
;LOOP
	;;NOP
	;LDR r4,[r0,r3,LSL #2]
	;STR r4,[r1,r3,LSL #2]
	;ADDS r3,r3,#1
;ENDLOOP
	;CMP r3,r2
	;BLT LOOP
	;POP {r4,pc}
	;ENDP
		
		
; 跳转函数  void *fun,u32 *stack, int len, u32 r[4]			
asm_runF    PROC
	EXPORT  asm_runF
	PUSH {R4-R7, lr}
	
	SUBS r4,r2,#1
	B asm_runF_ENDLOOP
asm_runF_LOOP
	;NOP
	LDR r5,[r1,r4,LSL #2]
	PUSH {r5}
	SUBS r4,r4,#1
asm_runF_ENDLOOP
	CMP r4,#0
	BGE asm_runF_LOOP
	
	MOV r5,r0
	MOV r6,r2
	MOV r7,r3
	;LDR r0,[r7]
	;LDR r1,[r7,#4]
	;LDR r2,[r7,#8]
	;LDR r3,[r7,#12]
	LDM r7,{r0-r3}
	
	BLX r5
	ADD sp,sp,r6,LSL #2
	POP {R4-R7, PC}
	ENDP
		

	
    END