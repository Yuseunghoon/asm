/*first.s*/
/*This is a comment */

.global main /* 'main' is our entry point and must be global */
main:
		mov r0, #2 /* put a 2 inside the register r0 */
				bx  lr /* return from main : r0 */
