.data

.balign 4
myvar1:
	.word 3

.text
.global main

main :

	mov r1, #1
	ldr r4, add_myvar1
	add r2, r1, r4
	mov r0, r2
	bx lr

	
add_myvar1 : .word myvar1
	
	
