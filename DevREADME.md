# !!! IMPORTANT !!!
Set up git **BEFORE** opening in vscode. Otherwise it will track your out/ and .gitignore won't work to ignore the files.

ONLY use CMake: Configure, CMake: Build, Cmake: Run without Debugging using ctrl-shift-p to run the application.
If you use the the built in Run button in VSCode it will bypass the Cmake properties and mess with the compiler used
by using clang-cl even though clang++ is configured to be used and will cause dozens of bogus errors.
Unless you want to spent 2 days fixing it like I did?

## Building and running for changes
For somereason if you use CMake: run without debugging before you CMake: Build after a change. It will run the Build 
process but get stuck making the embedded_manifest.h, so just manually CMake:Build after every change and test.

# Installations:
- Vim (for xxd)
- CMake
- Ninja
- LLVM (For clang and clang++) (Change CmakePresets to point to your clang/clang++ installation)

# When upgrading nmap Version via CMake FetchContent
Delete all contents in src/embedded/.
run CMake: Configure, this will run the configue phase to download nmap, create the embedded.cpp files and make the embedded_manifest.h which the build phase requires.
Then run CMake: Build, will build the cpp files to run.
