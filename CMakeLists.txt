# CMakeList.txt : CMake project for Discovr-GroupNumber, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.15)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("Discovr-GroupNumber")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#  === Download Nmap ===
include(FetchContent)
FetchContent_Declare(
	nmap_binary
	URL https://nmap.org/dist/nmap-7.92-win32.zip
	DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(nmap_binary)

# === Create output folder for embedded files ===
set(GENERATED_DIR ${CMAKE_SOURCE_DIR}/src/embedded)
file(MAKE_DIRECTORY ${GENERATED_DIR})

# === Convert Nmap binary into C array ===
set(NMAP_EXE "${nmap_binary_SOURCE_DIR}/nmap.exe")
set(NMAP_CPP "${GENERATED_DIR}/nmap_embedded.cpp")

add_custom_command(
	OUTPUT ${NMAP_CPP}
	COMMAND xxd -i ${NMAP_EXE} > ${NMAP_CPP}
	DEPENDS ${NMAP_EXE}
	COMMENT "Embedding Nmap binary into C++ source"
	VERBATIM
)

# === Wrap in a custom target that always builds ===
add_custom_target(embed_nmap ALL
    DEPENDS ${NMAP_CPP}
)

# Add source to this project's executable.
add_executable (Discovr-GroupNumber
	"Discovr-GroupNumber.cpp"
	"Discovr-GroupNumber.h"
	${NMAP_CPP}
)

# Ensure the embedded file is generated before building the executable
add_dependencies(Discovr-GroupNumber embed_nmap)
