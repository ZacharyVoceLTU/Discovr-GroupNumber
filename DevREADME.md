# IMPORTANT
ONLY use CMake: Build, Cmake: Run without Debugging.
If you use the the built in Run button in VSCode it will bypass the Cmake properties and mess with the compiler used
and cause dozens of bogus errors.

# When upgrading nmap Version via CMake FetchContent
Delete all contents in src/embedded/.
This will force CMake to embed the nmap.exe and dlls from the newly downloaded nmap.