
.text
.global main

main:
	mov r1, #2
	mov r2, #2
	cmp r1, r2
	beq end
	mov r0, #20
	bx lr
	
end:
	mov r0, #10
	bx lr
