# CHicago

## Build your own CHicago iso

You need:
	i686-elf toolchain
	grub2

Go to the root directory and type:

```
make
```

You should have the bootable iso inside of the build folder.

## Troubleshooting

### Barebone toolchain

To easy get toolchain to build this project you can clone:

https://github.com/rm-hull/barebones-toolchain.git

Read the repo README for instructions of toolchain versions and enviroment setup.

### No iso in build folder

Maybe some error during build. For debug it use:

```
make VERBOSE=true
```

Or:

```
make remake VERBOSE=true
```

### GRUB xorriso

CHicago use GRUB and GRUB tools to make bootable iso.
If you have any of these errors during build:

```
Your xorriso doesn't support ‘--grub2-boot-info’. Some features are disabled. Please use xorriso 1.2.9 or later
```

Make sure you have xorriso package installed in your distro.
