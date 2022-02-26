/* SPDX-License-Identifier: BSD-3-Clause */
.section ".note.GNU-stack","",@progbits
.text
.extern main, exit
.global _start, environ

# See System V ABI for more information
_start:
	mov (%rsp), %rdi         # Argument count starts at %rsp
	lea 8(%rsp), %rsi        # Argument pointers start at 8+%rsp
	lea 8(%rsi,%rdi,8), %rdx # Environment pointers start at 8+8*argc+%rsp
	mov %rdx, environ
	call main
	mov %rax, %rdi
	call exit

.comm environ, 8

