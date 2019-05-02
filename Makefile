
all: initramfs.cpio.gz linux/arch/x86_64/boot/bzImage

simu: all
#	qemu-system-x86_64 -kernel linux/arch/x86_64/boot/bzImage -initrd initramfs.cpio.gz
	qemu-system-x86_64 -nographic -no-reboot -kernel linux/arch/x86_64/boot/bzImage -initrd initramfs.cpio.gz -append 'panic=1 console=ttyS0' -hda hda.img

initramfs.cpio.gz:
	make -C initramfs

linux/.config:
	make -C linux x86_64_defconfig

linux/arch/x86_64/boot/bzImage: linux/.config
	make -C linux -j `nproc`

clean:
	rm -rf initramfs.cpio.gz initramfs/init

distclean: clean
	make -C linux distclean

