# initrfs

Minimal initramfs for a simplified boot sequence.

Usual initramfs tend to embed whole shells and utilities inside the filesystem.
Effectively providing a small operating system. Which is often overkill for a boot sequence.
To minimize initrd.img size and fasten the boot sequence, this initramfs doesn't embed modules
and executes a single binary executable. It then leverages the kernel's cmdline arguments to
mount the root filesystem. Once the root filesystem is mounted, it parses a config.sys file,
which provides fstab and modules arguments.

## Configure, build and test

Meson is used to configure, build and test a basic boot sequence.

```sh
meson setup build
meson compile -C build
# QEMU and mke2fs are required to run tests, see notes in test/meson.build
meson test -C build
```
