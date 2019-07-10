CC=musl-clang
CFLAGS=-O3 -Wall -fPIC
LDFLAGS=-static

CANDIDATES=./test/candidates

BUILDDIR=./build
INITRAMFS=$(BUILDDIR)/initramfs.cpio.gz
INITRAMFS_BIN=$(BUILDDIR)/bin/init
INITRAMFS_SRC=$(wildcard src/*.c)
TEST_BIN=$(BUILDDIR)/bin/test
TEST_SRC=test/test.c
TEST_IMAGE=$(BUILDDIR)/hda.img

LINUX=linux
LINUX_IMAGE=$(LINUX)/arch/x86_64/boot/bzImage

.PHONY: all test clean

all: $(INITRAMFS)

test: $(LINUX_IMAGE) $(INITRAMFS) $(TEST_IMAGE)
	qemu-system-x86_64 -nographic -no-reboot -kernel $(LINUX_IMAGE) -initrd $(INITRAMFS) -append 'panic=1 console=ttyS0' -hda $(TEST_IMAGE)

clean:
	rm -rf $(BUILDDIR)

$(BUILDDIR):
	mkdir -p $(BUILDDIR)/bin

$(LINUX):
	git submodule init
	git submodule update
	make -C $@ distclean x86_64_defconfig

$(LINUX_IMAGE): $(LINUX)
	make -C $< -j `nproc`

$(INITRAMFS): $(INITRAMFS_BIN) $(BUILDDIR)
	./tools/mkinitramfs -a $@ -i $< -c $(CANDIDATES)

$(INITRAMFS_BIN): $(INITRAMFS_SRC) $(BUILDDIR)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(INITRAMFS_SRC)

$(TEST_IMAGE): $(TEST_BIN) $(BUILDDIR)
	cp test/hda.img $@
	mount $@ /mnt
	cp $< /mnt/sbin/init
	umount /mnt

$(TEST_BIN): $(TEST_SRC) $(BUILDDIR)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(TEST_SRC)

