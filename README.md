[![Build Status](https://travis-ci.com/CHOSTeam/CHicago.svg?branch=master)](https://travis-ci.com/CHOSTeam/CHicago)
# CHicago

## Building the ISO

For building the ISO from scratch, you need:

	A Unix-like environment
	GCC and G++
	Make
	Bison
	Flex
	GMP
	MPFR
	MPC
	Texinfo
	Xorriso
	GNU-EFI (for EFI variants, like x86_32 or x86_64)
	MTools (for EFI variants, like x86_32 or x86_64)

To build it, go to the root directory and type:

	make

You can append ARCH=\<ARCH\> and SUBARCH=\<SUBARCH\> to change the system architecture, BUILD_CORES=\<CORES_AMOUNT\> to build the toolchain with multiple cores, VERBOSE=true to enable verbose build (good for finding compilation errors) and DEBUG=yes to disable optimizations and make a debug build.

After building everything, you should have your toolchain inside of the toolchain/\<ARCH\> folder and the bootable iso inside of the build folder (chicago-\<ARCH\>_\<SUBARCH\>.iso).
