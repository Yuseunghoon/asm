.text
.global main

main:
	mov r1, #0
	mov r2, #1
	
	mov r3, #22
loop:
	cmp r2, r3
	bgt end
	add r1, r1, r2
	add r2, r2, #1
	b loop
	
end:
	mov r0, r1
	bx lr
