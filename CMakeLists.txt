# CMakeList.txt : CMake project for Discovr-GroupNumber, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.15)

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

# === Create output folder for embedded nmap binary and dlls ===
set(EMBEDDED_DIR ${CMAKE_SOURCE_DIR}/src/embedded)
file(MAKE_DIRECTORY ${EMBEDDED_DIR})

# === Convert Nmap binary into C array ===
set(NMAP_EXE "${nmap_binary_SOURCE_DIR}/nmap.exe")
set(NMAP_CPP "${EMBEDDED_DIR}/nmap_embedded.cpp")
set(NMAP_STAMP "${CMAKE_BINARY_DIR}/nmap_embedded.stamp")

# === Convert nmap.exe to cpp ===
add_custom_command(
	OUTPUT ${NMAP_CPP}
	COMMAND xxd -i -n nmap_exe ${NMAP_EXE} > ${NMAP_CPP}
	DEPENDS ${NMAP_EXE}
	COMMENT "Embedding Nmap binary into C++ source"
	VERBATIM
)

# === nmap DLL folder ===
set(DLL_DIR "${nmap_binary_SOURCE_DIR}")

# === Collect nmap DLLs ===
file(GLOB DLL_FILES "${DLL_DIR}/*.dll")

# === Loop over each DLL and create a .cpp file ===
set(EMBEDDED_DLL_CPP_LIST "")
foreach(DLL ${DLL_FILES})
	get_filename_component(DLL_NAME ${DLL} NAME)
	set(DLL_CPP "${EMBEDDED_DIR}/${DLL_NAME}_embedded.cpp")

	add_custom_command(
		OUTPUT "${DLL_CPP}"
		COMMAND xxd -i -n ${DLL_NAME} "${DLL}" > "${DLL_CPP}"
		DEPENDS "${DLL}"
		COMMENT "Embedding ${DLL_NAME} into C++ source"
		VERBATIM
	)

	list(APPEND EMBEDDED_DLL_CPP_LIST "${DLL_CPP}")
endforeach()

# === Writing dll info to header ===
file(GLOB DLL_FILES "${nmap_binary_SOURCE_DIR}/*.dll")

# Generate a header with all the DLL info
set(OUTPUT_HEADER "${EMBEDDED_DIR}/dll_list.h")

add_custom_command(
	OUTPUT ${OUTPUT_HEADER}
	COMMAND ${CMAKE_COMMAND} 
		-DOUTPUT_HEADER=${OUTPUT_HEADER}
		-DDLL_DIR=${nmap_binary_SOURCE_DIR}
		-P "${CMAKE_SOURCE_DIR}/generate_dll_list.cmake"
	DEPENDS ${DLL_FILES}
	COMMENT "Generating DLL header for embedded Nmap DLLs"
	VERBATIM
)

# === Wrap in a custom target that always builds ===
add_custom_target(embed_nmap ALL
    DEPENDS ${NMAP_CPP}
)

add_custom_target(embed_dlls ALL
	DEPENDS ${EMBEDDED_DLL_CPP_LIST}
)

add_custom_target(embed_dlls_header ALL
	DEPENDS ${OUTPUT_HEADER}
)

# Add source to this project's executable.
add_executable (Discovr-GroupNumber
	"src/Discovr-GroupNumber.cpp"
	${NMAP_CPP}
	${EMBEDDED_DLL_CPP_LIST}
	"src/extractors/Extractor.h"
	"src/extractors/NmapExtractor.cpp"
	"src/extractors/NmapExtractor.h"
	"src/extractors/DllExtractor.h"
	"src/extractors/DllExtractor.cpp")

# Ensure the embedded file is generated before building the executable
add_dependencies(Discovr-GroupNumber embed_nmap)
add_dependencies(Discovr-GroupNumber embed_dlls)
add_dependencies(Discovr-GroupNumber embed_dlls_header)
