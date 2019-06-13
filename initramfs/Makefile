CC=clang

../initramfs.cpio.gz: init dev proc mnt candidates
	find $^ | cpio -oH newc | gzip -9 > $@

dev proc mnt:
	mkdir $@

init: $(wildcard ../src/*.c)
	$(CC) -static -o $@ $^

