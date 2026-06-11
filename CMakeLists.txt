#
# CMake wrapper for the Windows / AppVeyor build of the TOPPERS/HRP2 (EV3)
# .NET Micro Framework projects.
#
# This reproduces, via CMake, the same steps as appveyor.yml's build_script:
#
#   setenv_gcc <ver> <gcc-arm-none-eabi-dir>
#   cd Solutions\ToppersHRP2onEV3
#   MSBuild dotnetmf.proj /t:rebuild /p:flavor=release;memory=flash
#   cd ..\..
#   cd DeviceCode\Targets\OS\Toppers\hrp2\workspace\helloev3onnetmf
#   MSBuild dotnetmf.proj /t:rebuild /p:flavor=release;memory=flash
#
# i.e. it drives the existing MSBuild-based toolchain
# (tools/Targets/Toppers.Spot.system.gcc.targets), which already knows how to
# invoke arm-none-eabi-gcc/cfg/etc. CMake's job here is purely environment
# setup (locating the GCC ARM toolchain and MSBuild) and orchestration.
#
# For a build that instead drives the GNU Make + cfg.exe based kernel build
# directly, see DeviceCode/Targets/OS/Toppers/hrp2/CMakeLists.txt.
#
cmake_minimum_required(VERSION 3.15)
project(MicroFrameworkPK_ToppersEV3_AppVeyor NONE)

if(NOT WIN32)
    message(FATAL_ERROR
        "This project drives MSBuild and the .NET Micro Framework build "
        "system, which are Windows-only.")
endif()

#-------------------------------------------------------------------------
# Options
#-------------------------------------------------------------------------
set(TOPPERS_GCC_PREFIX "arm-none-eabi" CACHE STRING
    "GNU cross toolchain prefix (GNU_TARGET)")
set(TOPPERS_FLAVOR "release" CACHE STRING
    "MSBuild flavor (release/debug)")

#-------------------------------------------------------------------------
# Locate the GNU Arm Embedded toolchain (same as appveyor.yml's
# gcc-arm-none-eabi download, "setenv_gcc <ver> <dir>")
#-------------------------------------------------------------------------
find_program(TOPPERS_ARM_GCC
    NAMES ${TOPPERS_GCC_PREFIX}-gcc.exe ${TOPPERS_GCC_PREFIX}-gcc
    HINTS
        "C:/Program Files (x86)/GNU Arm Embedded Toolchain"
        "C:/Program Files (x86)/GNU Tools ARM Embedded"
        "C:/Program Files/GNU Arm Embedded Toolchain"
        "C:/SysGCC/arm-eabi"
        "C:/projects/gcc-arm-none-eabi-4_9"
    PATH_SUFFIXES bin
)

if(NOT TOPPERS_ARM_GCC)
    message(FATAL_ERROR
        "Could not find '${TOPPERS_GCC_PREFIX}-gcc'.\n"
        "Install the GNU Arm Embedded Toolchain "
        "(https://developer.arm.com/downloads/-/gnu-rm), e.g. the same "
        "gcc-arm-none-eabi-4_9-2014q4 build used by appveyor.yml, and add "
        "its 'bin' directory to PATH, or pass:\n"
        "  cmake -DCMAKE_PROGRAM_PATH=\"<toolchain>/bin\" ...")
endif()

get_filename_component(GNU_TOOLS_BIN "${TOPPERS_ARM_GCC}" DIRECTORY)
file(TO_NATIVE_PATH "${GNU_TOOLS_BIN}" GNU_TOOLS_BIN)
message(STATUS "AppVeyor build: GNU_TOOLS_BIN=${GNU_TOOLS_BIN}")

#-------------------------------------------------------------------------
# Locate MSBuild (.NET Framework 4.0, as used by appveyor's VS2013 image)
#-------------------------------------------------------------------------
find_program(TOPPERS_MSBUILD
    NAMES MSBuild.exe
    HINTS
        "$ENV{WINDIR}/Microsoft.NET/Framework64/v4.0.30319"
        "$ENV{WINDIR}/Microsoft.NET/Framework/v4.0.30319"
)

if(NOT TOPPERS_MSBUILD)
    # Fall back to vswhere (Visual Studio 2017+)
    find_program(TOPPERS_VSWHERE vswhere.exe
        HINTS "C:/Program Files (x86)/Microsoft Visual Studio/Installer")
    if(TOPPERS_VSWHERE)
        execute_process(
            COMMAND "${TOPPERS_VSWHERE}" -latest -requires Microsoft.Component.MSBuild
                    -find "MSBuild\\**\\Bin\\MSBuild.exe"
            OUTPUT_VARIABLE TOPPERS_MSBUILD_VSWHERE
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        if(TOPPERS_MSBUILD_VSWHERE)
            string(REPLACE "\r\n" ";" _msbuild_list "${TOPPERS_MSBUILD_VSWHERE}")
            list(GET _msbuild_list 0 TOPPERS_MSBUILD)
        endif()
    endif()
endif()

if(NOT TOPPERS_MSBUILD)
    message(FATAL_ERROR
        "Could not find MSBuild.exe.\n"
        "Install the .NET Framework 4.0+ Developer Pack (provides "
        "%WINDIR%\\Microsoft.NET\\Framework\\v4.0.30319\\MSBuild.exe), or "
        "Visual Studio (any edition with the MSBuild component).")
endif()
message(STATUS "AppVeyor build: MSBuild=${TOPPERS_MSBUILD}")

#-------------------------------------------------------------------------
# Generate a batch script reproducing appveyor.yml's build_script and wire
# it up as a CMake target.
#-------------------------------------------------------------------------
file(TO_NATIVE_PATH "${CMAKE_CURRENT_SOURCE_DIR}" TOPPERS_CLRROOT)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/run_appveyor_build.cmd.in"
    "${CMAKE_CURRENT_BINARY_DIR}/run_appveyor_build.cmd"
    @ONLY
)

add_custom_target(toppers_ev3_appveyor ALL
    COMMAND "${CMAKE_CURRENT_BINARY_DIR}/run_appveyor_build.cmd"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    COMMENT "Building ToppersHRP2onEV3 + helloev3onnetmf via MSBuild (appveyor.yml equivalent)"
    USES_TERMINAL
    VERBATIM
)
