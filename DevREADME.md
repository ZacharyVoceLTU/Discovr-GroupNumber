# !!! IMPORTANT !!!
Set up git **BEFORE** opening in vscode. Otherwise it will track your out/ and .gitignore won't work to ignore the files.

ONLY use CMake: Build, Cmake: Run without Debugging using ctrl-shift-p.
If you use the the built in Run button in VSCode it will bypass the Cmake properties and mess with the compiler used
by using clang-cl even though clang++ is configured to be used and cause dozens of bogus errors.

# Installations:
- xxd
- CMake
- Ninja
- LLVM

# When upgrading nmap Version via CMake FetchContent
Delete all contents in src/embedded/.
This will force CMake to embed the nmap.exe and dlls from the newly downloaded nmap.
