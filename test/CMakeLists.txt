
set(TEST_BZIMAGE "/vmlinuz" CACHE STRING "Linux kernel image used to run tests.")
set(TEST_QEMU_SYSTEM "qemu-system-${CMAKE_SYSTEM_PROCESSOR}" CACHE STRING "QEMU system emulator to run tests.")
set(TEST_QEMU_OPTIONS "" CACHE STRING "QEMU system emulator additional options.")

add_custom_target(test-initrd ALL DEPENDS initrd.img)
add_custom_command(
	OUTPUT initrd.img
	COMMAND mkinitrfs -I ${PROJECT_BINARY_DIR}/init initrd.img
	DEPENDS mkinitrfs init
)

add_custom_target(test-disk ALL DEPENDS disk.img)
add_custom_command(
	OUTPUT disk.img
	COMMAND mke2fs -q -I 256 -t ext4 -d rootfs disk.img 5M
	DEPENDS test-init
)

add_executable(test-init support/test-init/main.c)
set_target_properties(test-init PROPERTIES OUTPUT_NAME rootfs/sbin/init)
target_compile_options(test-init PRIVATE -nostdinc -ffreestanding)
target_link_libraries(test-init PRIVATE initcrt -nostdlib -static)
add_dependencies(test-init test-rootfs)

add_custom_target(test-rootfs DEPENDS rootfs)
add_custom_command(
	OUTPUT rootfs
	COMMAND cp -R -- ${CMAKE_CURRENT_SOURCE_DIR}/support/test-rootfs rootfs
	COMMAND mkdir -p rootfs/sbin rootfs/dev rootfs/proc rootfs/sys
)

add_test(NAME boot COMMAND ${TEST_QEMU_SYSTEM} ${TEST_QEMU_OPTIONS} -no-reboot -serial stdio -append "panic=-1 console=ttyS0 root=/dev/sda rootfstype=ext4 rootflags=rw,relatime,data=ordered" -kernel ${TEST_BZIMAGE} -initrd initrd.img -drive format=raw,file=disk.img)

