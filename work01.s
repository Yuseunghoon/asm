.data


.balign 4
message1: .asciz "put two number : "

.balign 4
return: .word 0

.balign 4
return2: .word 0

.balign 4
return3: .word 0

.balign 4
return4: .word 0

.balign 4
return5: .word 0

.balign 4
return6: .word 0

.balign 4
scan_pattern: .asciz "%d %d"

.balign 4
number_read_1: .word 0

.balign 4
number_read_2: .word 0

.balign 4
message2: .asciz "%d %d\n"

.balign 4
message_add: .asciz "add : %d\n"

.balign 4
message_sub: .asciz "sub : %d\n"

.balign 4
message_mul: .asciz "mul : %d\n"

.balign 4
message_div: .asciz "div : %d\n"

.balign 4
message_wrong: .asciz "wrong : %d\n"


.text

add:
	ldr r4, address_of_return2
	str lr, [r4]

	add r1, r1, r2
	
	ldr r0, address_of_message_add
	bl printf
	
	ldr lr, address_of_return2
	ldr lr, [lr]
	bx lr
	
	address_of_return2 : .word return2	

sub:
	ldr r4, address_of_return3
	str lr, [r4]

	sub r1, r1, r2
	
	ldr r0, address_of_message_sub
	bl printf
	
	ldr lr, address_of_return3
	ldr lr, [lr]
	bx lr
	
	address_of_return3 : .word return3	

mul:
	ldr r4, address_of_return4
	str lr, [r4]
	
	mul r1, r2
	
	ldr r0, address_of_message_mul
	bl printf
	
	ldr lr, address_of_return4
	ldr lr, [lr]
	bx lr
	
	address_of_return4 : .word return4	
	
div:
	ldr r4, address_of_return5
	str lr, [r4]
	
	cmp r2, #0
	beq wrong

	sdiv r1, r1, r2
	
	
	ldr r0, address_of_message_div
	bl printf
	
	ldr lr, address_of_return5
	ldr lr, [lr]
	bx lr
	
	address_of_return5 : .word return5	
	


	
.global main
	main:
	ldr r1, address_of_return
	str lr, [r1]
	
	ldr r0, address_of_message1
	bl printf
	
	ldr r0, address_of_scan_pattern
	ldr r1, address_of_number_read_1
	ldr r2, address_of_number_read_2
	bl scanf 
	
	cmp r0, #2
	bne wrong
	
	ldr r1, address_of_number_read_1
	ldr r1, [r1]
	ldr r2, address_of_number_read_2
	ldr r2, [r2]
	ldr r0, address_of_message2
	bl printf
	
	ldr r1, address_of_number_read_1
	ldr r1, [r1]
	ldr r2, address_of_number_read_2
	ldr r2, [r2]
	ldr r0, address_of_message2
	bl add
	
	ldr r1, address_of_number_read_1
	ldr r1, [r1]
	ldr r2, address_of_number_read_2
	ldr r2, [r2]
	ldr r0, address_of_message2
	bl sub
	
	ldr r1, address_of_number_read_1
	ldr r1, [r1]
	ldr r2, address_of_number_read_2
	ldr r2, [r2]
	ldr r0, address_of_message2
	bl mul
	
	ldr r1, address_of_number_read_1
	ldr r1, [r1]
	ldr r2, address_of_number_read_2
	ldr r2, [r2]
	ldr r0, address_of_message2
	bl div
	
	mov r0, #0
	
	ldr lr, address_of_return
	ldr lr, [lr]	

	bx lr


wrong:
	
	mvn r0, #-1
	add r0, r0, #1
	
		
	ldr lr, address_of_return
	ldr lr, [lr]

	bx lr


address_of_message1 : .word message1
address_of_return : .word return
address_of_scan_pattern : .word scan_pattern
address_of_number_read_1 : .word number_read_1
address_of_number_read_2 : .word number_read_2
address_of_message2 : .word message2
address_of_message_add : .word message_add
address_of_message_sub : .word message_sub
address_of_message_mul : .word message_mul
address_of_message_div : .word message_div
address_of_message_wrong : .word message_wrong


