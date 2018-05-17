.data
.balign 4
a: .skip 400

.text
.global main

main:
	ldr r1, addr_of_a
	mov r2, #0

/* 

r2=0 
r3= r1(addr_of_a)+r2*4// addr
*r3<-r2  // r3=a[i]
r2=r2+1 -> r2=1

*/
loop:
	cmp r2, #100
	beq end
	add r3, r1, r2, LSL #2
	
	mov r4, r2
	add r4, r2, LSL #1
	add r4, r4, #1
	
	str r4, [r3]
	add r2, r2, #1
	b loop

end:
	bx lr
	
addr_of_a: .word a
