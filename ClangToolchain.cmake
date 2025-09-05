# This file is a CMake toolchain file for Clang on Windows.

set(CMAKE_C_COMPILER "C:/Program Files/LLVM/bin/clang.exe")
set(CMAKE_CXX_COMPILER "C:/Program Files/LLVM/bin/clang++.exe")
# This sets the resource compiler to the one provided with LLVM
set(CMAKE_RC_COMPILER "C:/Program Files/LLVM/bin/llvm-rc.exe" CACHE FILEPATH "The Resource Compiler.")
set(CMAKE_C_COMPILER_TARGET "x86_64-pc-windows-msvc")
set(CMAKE_CXX_COMPILER_TARGET "x86_64-pc-windows-msvc")

# Linker and archiver from LLVM
set(CMAKE_LINKER "C:/Program Files/LLVM/bin/lld-link.exe")
set(CMAKE_AR "C:/Program Files/LLVM/bin/llvm-lib.exe")

# This is the crucial part that tells IntelliSense where to find the standard library headers
# You can get these paths by running: "C:\Program Files\LLVM\bin\clang++.exe" -v -c -x c++ nul
# The paths below are a common example, you may need to adjust them based on your setup.
set(CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES
    "C:/Program Files/LLVM/lib/clang/21/include"
    "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.44.35207/include"
    "C:/Program Files (x86)/Windows Kits/10/Include/10.0.26100.0/ucrt"
    "C:/Program Files (x86)/Windows Kits/10/Include/10.0.26100.0/um"
    "C:/Program Files (x86)/Windows Kits/10/Include/10.0.26100.0/shared"
    "C:/Program Files (x86)/Windows Kits/10/Include/10.0.26100.0/winrt"
)