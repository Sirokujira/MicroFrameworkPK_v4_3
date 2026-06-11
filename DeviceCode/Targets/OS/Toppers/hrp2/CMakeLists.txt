#
# CMake build environment setup for TOPPERS/HRP2 (EV3 / am1808, ev3_gcc target)
#
# This project does not re-implement the HRP2 GNU Make build (it is a deeply
# multi-pass build driven by the TOPPERS configurator, "cfg"/"cfg.exe").
# Instead, it locates the tools required on Windows and drives the existing
# Makefile through them, so that the kernel can be built with:
#
#     cmake -S . -B build -G "Ninja"   (or any generator; no compiler needed)
#     cmake --build build
#
# Required tools (Windows):
#   - GNU Arm Embedded Toolchain (arm-none-eabi-gcc and friends)
#       https://developer.arm.com/downloads/-/gnu-rm
#   - A POSIX shell + coreutils (rm/touch/diff/cp) used by the Makefile recipes
#       e.g. Git for Windows ("Git Bash") or MSYS2
#   - GNU Make (mingw32-make from MSYS2, or make from Git for Windows/MSYS2)
#   - cfg.exe (TOPPERS configurator), already checked in under cfg/cfg/cfg.exe
#
cmake_minimum_required(VERSION 3.15)
project(ToppersHRP2_EV3 NONE)

#-------------------------------------------------------------------------
# Options
#-------------------------------------------------------------------------
set(HRP2_TARGET "ev3_gcc" CACHE STRING
    "TOPPERS/HRP2 target board directory under target/ (Makefile TARGET=)")
set(HRP2_GCC_PREFIX "arm-none-eabi" CACHE STRING
    "GNU cross toolchain prefix (Makefile GCC_TARGET)")
set(HRP2_MAKE_JOBS "1" CACHE STRING
    "Number of parallel make jobs (-j)")

#-------------------------------------------------------------------------
# Locate the GNU Arm Embedded toolchain
#-------------------------------------------------------------------------
find_program(HRP2_ARM_GCC
    NAMES ${HRP2_GCC_PREFIX}-gcc ${HRP2_GCC_PREFIX}-gcc.exe
    HINTS
        "C:/Program Files (x86)/GNU Arm Embedded Toolchain"
        "C:/Program Files (x86)/GNU Tools ARM Embedded"
        "C:/Program Files/GNU Arm Embedded Toolchain"
        "C:/SysGCC/arm-eabi"
    PATH_SUFFIXES bin
)

if(NOT HRP2_ARM_GCC)
    message(FATAL_ERROR
        "Could not find '${HRP2_GCC_PREFIX}-gcc'.\n"
        "Install the GNU Arm Embedded Toolchain "
        "(https://developer.arm.com/downloads/-/gnu-rm) and either add its "
        "'bin' directory to PATH, or pass it explicitly, e.g.:\n"
        "  cmake -DCMAKE_PROGRAM_PATH=\"C:/Program Files (x86)/GNU Arm Embedded Toolchain/10 2021.10/bin\" ...")
endif()

get_filename_component(HRP2_TOOLCHAIN_BINDIR "${HRP2_ARM_GCC}" DIRECTORY)
message(STATUS "HRP2: using ARM toolchain at ${HRP2_TOOLCHAIN_BINDIR}")

#-------------------------------------------------------------------------
# Locate GNU Make
#-------------------------------------------------------------------------
find_program(HRP2_MAKE
    NAMES mingw32-make mingw32-make.exe make make.exe
    HINTS
        "C:/msys64/usr/bin"
        "C:/msys64/mingw64/bin"
        "C:/Program Files/Git/usr/bin"
)

if(NOT HRP2_MAKE)
    message(FATAL_ERROR
        "Could not find GNU Make ('make' or 'mingw32-make').\n"
        "Install MSYS2 (https://www.msys2.org/) and run:\n"
        "  pacman -S make\n"
        "or install Git for Windows, which bundles 'usr/bin/make.exe'.")
endif()
message(STATUS "HRP2: using make at ${HRP2_MAKE}")

#-------------------------------------------------------------------------
# Locate a POSIX shell (the HRP2 Makefile recipes use rm/touch/diff/cp)
#-------------------------------------------------------------------------
find_program(HRP2_SHELL
    NAMES bash bash.exe sh sh.exe
    HINTS
        "C:/msys64/usr/bin"
        "C:/Program Files/Git/bin"
        "C:/Program Files/Git/usr/bin"
        "C:/MinGW/msys/1.0/bin"
)

if(NOT HRP2_SHELL)
    message(FATAL_ERROR
        "Could not find a POSIX shell (bash/sh) needed by the HRP2 Makefile.\n"
        "Install Git for Windows (https://git-scm.com/download/win) or MSYS2 "
        "(https://www.msys2.org/) so that 'bash' is available.")
endif()
message(STATUS "HRP2: using shell at ${HRP2_SHELL}")

#-------------------------------------------------------------------------
# Locate the TOPPERS configurator (cfg). A pre-built cfg.exe for Windows is
# checked in under cfg/cfg/. The Makefile's $(CFG) variable is overridden on
# the command line to point at it explicitly.
#-------------------------------------------------------------------------
if(WIN32)
    set(HRP2_CFG_DEFAULT "cfg/cfg/cfg.exe")
else()
    set(HRP2_CFG_DEFAULT "cfg/cfg/cfg")
endif()

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${HRP2_CFG_DEFAULT}")
    message(FATAL_ERROR
        "Could not find the TOPPERS configurator at '${HRP2_CFG_DEFAULT}'.\n"
        "On non-Windows hosts, build cfg from cfg/cfg/ first, or run this "
        "build under Windows where the prebuilt cfg.exe is provided.")
endif()
message(STATUS "HRP2: using configurator ${HRP2_CFG_DEFAULT}")

#-------------------------------------------------------------------------
# Build / clean targets: drive the existing Makefile through the located
# toolchain, shell and configurator.
#-------------------------------------------------------------------------
file(TO_NATIVE_PATH "${HRP2_TOOLCHAIN_BINDIR}" HRP2_TOOLCHAIN_BINDIR_NATIVE)

set(HRP2_MAKE_ARGS
    SHELL=${HRP2_SHELL}
    CFG=${HRP2_CFG_DEFAULT}
    GCC_TARGET=${HRP2_GCC_PREFIX}
    TARGET=${HRP2_TARGET}
)

add_custom_target(hrp2 ALL
    COMMAND ${CMAKE_COMMAND} -E env "PATH=${HRP2_TOOLCHAIN_BINDIR_NATIVE}$<SEMICOLON>$ENV{PATH}"
            ${HRP2_MAKE} ${HRP2_MAKE_ARGS} -j${HRP2_MAKE_JOBS}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Building TOPPERS/HRP2 (TARGET=${HRP2_TARGET}) via GNU Make"
    USES_TERMINAL
    VERBATIM
)

add_custom_target(hrp2_clean
    COMMAND ${CMAKE_COMMAND} -E env "PATH=${HRP2_TOOLCHAIN_BINDIR_NATIVE}$<SEMICOLON>$ENV{PATH}"
            ${HRP2_MAKE} ${HRP2_MAKE_ARGS} clean
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Cleaning TOPPERS/HRP2 (TARGET=${HRP2_TARGET})"
    USES_TERMINAL
    VERBATIM
)

add_custom_target(hrp2_realclean
    COMMAND ${CMAKE_COMMAND} -E env "PATH=${HRP2_TOOLCHAIN_BINDIR_NATIVE}$<SEMICOLON>$ENV{PATH}"
            ${HRP2_MAKE} ${HRP2_MAKE_ARGS} realclean
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Removing all generated files for TOPPERS/HRP2 (TARGET=${HRP2_TARGET})"
    USES_TERMINAL
    VERBATIM
)

# Convenience targets for the binary / S-record images
add_custom_target(hrp2_bin
    COMMAND ${CMAKE_COMMAND} -E env "PATH=${HRP2_TOOLCHAIN_BINDIR_NATIVE}$<SEMICOLON>$ENV{PATH}"
            ${HRP2_MAKE} ${HRP2_MAKE_ARGS} hrp2.bin
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Building hrp2.bin"
    USES_TERMINAL
    VERBATIM
)

add_custom_target(hrp2_srec
    COMMAND ${CMAKE_COMMAND} -E env "PATH=${HRP2_TOOLCHAIN_BINDIR_NATIVE}$<SEMICOLON>$ENV{PATH}"
            ${HRP2_MAKE} ${HRP2_MAKE_ARGS} hrp2.srec
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Building hrp2.srec"
    USES_TERMINAL
    VERBATIM
)

message(STATUS "HRP2: build output will be '${CMAKE_CURRENT_SOURCE_DIR}/hrp2' (use 'hrp2_bin'/'hrp2_srec' targets for .bin/.srec)")
