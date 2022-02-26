qemu_system = find_program('qemu-system-' + host_machine.cpu_family(), required : false)
mke2fs = find_program('mke2fs', '/sbin/mke2fs', '/usr/sbin/mke2fs', required : false)

if qemu_system.found() and mke2fs.found()
	# initramfs image, generated from our binaries and utilities
	initrd = custom_target('initrd',
		output : 'initrd.img', input : init,
		command : [ mkinitrfs, '-i', '@INPUT@', '--', '@OUTPUT@' ],
		depends : [ init, mkinitrfs ]
	)

	# boot /sbin/init executable, prints TAP output for meson test bench to receive
	boot = executable('boot',
		build_by_default : false,
		include_directories : headers,
		link_args : [ '-static' ],
		link_with : libinitcrt,
		sources : 'support/boot.c'
	)

	# sysroot hierarchy, built from config.sys and boot init executable
	sysroot = custom_target('sysroot',
		output : 'sysroot', input : [ 'support/config.sys', boot ],
		command : [ 'tools/mksysroot', '@OUTPUT@', '@INPUT0@', '@INPUT1@' ],
		depends : boot
	)

	# disk image used for the test, built from sysroot as ext4
	# NB: On most Linux distros, the default /vmlinuz does not embed
	# the ext4 filesystem driver, which raises the following error:
	# "Unable to mount '/dev/sda' (ext4) to '/mnt': No such device"
	disk = custom_target('disk',
		output : 'disk.img', input : sysroot,
		command : [ mke2fs, '-q', '-I', '256', '-t', 'ext4', '-d', '@INPUT@', '@OUTPUT@', '5M' ],
		# We follow each file dependencies + sysroot because it seems file dependencies are not forwarded.
		# Thus, we need to explicit them. sysroot is specified to avoid parallel build problems if sysroot is rebuilt at the same time.
		depend_files : 'support/config.sys',
		depends : [ boot, sysroot ]
	)

	# QEMU test execution, note -nographic prints out BIOS infos on stdio,
	# which we don't want because they pollute our TAP protocol output.
	# This is a real problem for automation of tests. Any fix is welcome.
	# Same for kernel logs, if an error occurs, you may want to set the 'loglevel' kernel argument
	# to something else than 0, see the kernel documentation for more information on loglevels
	test('boot', qemu_system,
		args : [
			'-no-reboot', '-serial', 'stdio',
			'-append', 'loglevel=0 panic=-1 console=ttyS0 root=/dev/sda rootfstype=ext4 rootflags=rw,relatime,data=ordered',
			'-kernel', get_option('vmlinuz'),
			'-initrd', initrd,
			'-drive', 'format=raw,file=' + disk.full_path(),
		],
		depends : [ initrd, disk ],
		protocol : 'tap'
	)
endif
