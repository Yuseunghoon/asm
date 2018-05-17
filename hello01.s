.data

greeting:
	.asciz "Hello world"
	
.balign 4
return: .word 0

.text

.global main
main:
	ldr r1, address_of_return /* r1 <- &address_of_return */
	str lr, [r1] /* *r1 <- lr */
	
	ldr r0, address_of_greeting /* r0 <- &address_of_greeting */
															/*  First parameter of puts */
	
	bl puts /* call to puts*/
					/*  lr <- address of next instruction */
					/*  put 함수 시 링크가 덮어써지기 때문에 백업 필요*/
	ldr r1, address_of_return /* r1 <- &addres_of_return */
	ldr lr, [r1] /* lr <- *r1*/
	bx lr /* return from main */

address_of_greeting: .word greeting
address_of_return: .word return

/*External*/
.global puts
