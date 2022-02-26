/* SPDX-License-Identifier: BSD-3-Clause */
.section ".note.GNU-stack","",@progbits
.text

/* NOTE: These syscalls shims do the following tasks:
 * - Translate returned values to errno for all but _exit
 * - As the SysV ABI third argument is passed through %rcx and not %r10,
 *   it moves the argument for requiring syscalls (mmap, mount).
 */

.global read
read:
	mov $0, %rax
	syscall
	mov %rax, %rdi
	jmp _propagate_errno

.global write
write:
	mov $1, %rax
	syscall
	mov %rax, %rdi
	jmp _propagate_errno

.global open
open:
	mov $2, %rax
	syscall
	mov %rax, %rdi
	jmp _propagate_errno

.global close
close:
	mov $3, %rax
	syscall
	mov %rax, %rdi
	jmp _propagate_errno

.global mmap
mmap:
	mov $9, %rax
	mov %rcx, %r10
	syscall
	mov %rax, %rdi
	jmp _propagate_errno

.global munmap
munmap:
	mov $11, %rax
	syscall
	mov %rax, %rdi
	jmp _propagate_errno

.global nanosleep
nanosleep:
	mov $35, %rax
	syscall
	mov %rax, %rdi
	jmp _propagate_errno

.global execve
execve:
	mov $59, %rax
	syscall
	mov %rax, %rdi
	jmp _propagate_errno

.global _exit
_exit:
	mov $60, %rax
	syscall

.global chdir
chdir:
	mov $80, %rax
	syscall
	mov %rax, %rdi
	jmp _propagate_errno

.global chroot
chroot:
	mov $161, %rax
	syscall
	mov %rax, %rdi
	jmp _propagate_errno

.global mount
mount:
	mov $165, %rax
	mov %rcx, %r10
	syscall
	mov %rax, %rdi
	jmp _propagate_errno

.global umount2
umount2:
	mov $166, %rax
	syscall
	mov %rax, %rdi
	jmp _propagate_errno

.global reboot
reboot:
	mov $169, %rax
	mov %rdi, %rdx
	mov $0xfee1dead, %rdi
	mov $672274793, %rsi
	xor %r10, %r10
	syscall
	mov %rax, %rdi
	jmp _propagate_errno

.global getdents64
getdents64:
	mov $217, %rax
	syscall
	mov %rax, %rdi
	jmp _propagate_errno

.global openat
openat:
	mov $257, %rax
	xor %r10, %r10
	syscall
	mov %rax, %rdi
	jmp _propagate_errno

.global unlinkat
unlinkat:
	mov $263, %rax
	syscall
	mov %rax, %rdi
	jmp _propagate_errno

