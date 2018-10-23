# CHicago

## Build your own CHicago iso

You need: i686-elf toolchain, xorriso, mono and make

Go to the root directory and type:

```
make
```

You should have the bootable iso inside of the build folder and the blasm tool inside of tools/blasm/build (blasm.exe, you can run it with mono).

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
