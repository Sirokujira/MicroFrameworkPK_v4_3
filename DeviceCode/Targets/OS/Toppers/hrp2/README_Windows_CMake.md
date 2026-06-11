# Windows build via CMake

This directory's kernel build is normally driven by GNU Make together with
the TOPPERS configurator (`cfg`/`cfg.exe`). The `CMakeLists.txt` in this
directory does not reimplement that build (it is a multi-pass build that
generates `kernel_cfg.*`, `kernel_mem*.c` and linker scripts via `cfg`); it
locates the required tools on Windows and drives the existing `Makefile`
through them.

## Prerequisites

1. **GNU Arm Embedded Toolchain** (`arm-none-eabi-gcc` and friends)
   https://developer.arm.com/downloads/-/gnu-rm
2. **MSYS2** or **Git for Windows** — provides `bash`/`sh`, `make` (or
   `mingw32-make`) and the coreutils (`rm`, `touch`, `diff`, `cp`) used by
   the Makefile recipes.
3. **CMake** (3.15+).
4. The TOPPERS configurator `cfg.exe` is already checked into
   `cfg/cfg/cfg.exe` — no separate build needed.

## Build

```
cmake -S . -B build
cmake --build build --target hrp2
```

Available targets:

- `hrp2` (default/`ALL`) — builds the `hrp2` ELF image (TOPPERS sample1 app
  for `TARGET=ev3_gcc`).
- `hrp2_bin` / `hrp2_srec` — also produce `hrp2.bin` / `hrp2.srec`.
- `hrp2_clean` / `hrp2_realclean` — clean generated files.

Outputs are written into the source tree (`hrp2`, `hrp2.bin`, `hrp2.srec`,
etc.), matching the existing Makefile's in-source build layout.

## Options

- `-DHRP2_TARGET=ev3_gcc` — selects the `target/<name>` board directory
  (Makefile `TARGET=`).
- `-DHRP2_GCC_PREFIX=arm-none-eabi` — cross toolchain prefix.
- `-DHRP2_MAKE_JOBS=4` — parallel make jobs.

If CMake cannot locate the toolchain, `make`, or a shell automatically, add
their directories to `PATH` or pass `-DCMAKE_PROGRAM_PATH=...` pointing at
the toolchain's `bin` directory.
