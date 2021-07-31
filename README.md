# initrfs

Minimal initramfs for a simplified boot sequence.

Usual initramfs tend to embed whole shells and utilities inside the filesystem.
Effectively providing a small operating system. Which is often overkill for a boot sequence.
To minimize initrd.img size and fasten the boot sequence, this initramfs doesn't embed modules
and executes a single binary executable. It then leverages the kernel's cmdline arguments to
mount the root filesystem. Once the root filesystem is mounted, it parses a config.sys file,
which provides fstab and modules arguments.

## Configure, build and install

CMake is used to configure, build and install binaires and documentations, version 3.14 minimum is required:

```sh
mkdir -p build && cd build
cmake ../
cmake --build .
cmake --install .
```

## Tests

There is no real automated test infrastructure yet.
But you can specify a qemu-system program to verify the validity of the boot sequence.
Note: mke2fs is used to create the disk image, it is usually located in /sbin, which may not be in your default PATH.

```sh
mkdir -p build && cd build
cmake ../
cmake --build .
ctest -V
```

