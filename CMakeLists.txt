# CMakeList.txt : CMake project for Discovr-GroupNumber, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.20)

project ("Discovr-GroupNumber" LANGUAGES CXX)

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

# === Master list of all files to embed ===
set(EMBEDDED_DIR ${CMAKE_SOURCE_DIR}/src/NmapEmbedded)
file(MAKE_DIRECTORY "${EMBEDDED_DIR}")

# Get a list of all files to embed. Be specific to avoid directories and unwanted files.
file(GLOB NMAP_EXE "${nmap_binary_SOURCE_DIR}/nmap.exe")
file(GLOB DLL_FILES "${nmap_binary_SOURCE_DIR}/*.dll")
file(GLOB_RECURSE NMAP_SCRIPTS_FILES "${nmap_binary_SOURCE_DIR}/scripts/*.nse")
file(GLOB_RECURSE NMAP_NSELIB_FILES "${nmap_binary_SOURCE_DIR}/nselib/*")

# Combine all lists into a single master list
set(MASTER_EMBED_FILES ${NMAP_EXE} ${DLL_FILES} ${NMAP_SCRIPTS_FILES} ${NMAP_NSELIB_FILES} ${NMAP_NSELIB_DATA_FILES})

set(EMBEDDED_CPP_LIST "")

# === Single loop to process all files ===
foreach(FILE_TO_EMBED ${MASTER_EMBED_FILES})
    file(RELATIVE_PATH REL_PATH ${nmap_binary_SOURCE_DIR} ${FILE_TO_EMBED})
    string(REPLACE "/" "_" VAR_NAME_PATH ${REL_PATH})
    string(REPLACE "-" "_" VAR_NAME ${VAR_NAME_PATH})
    string(REPLACE "." "_" VAR_NAME ${VAR_NAME})
    set(FULL_OUTPUT_PATH "${EMBEDDED_DIR}/${REL_PATH}_embedded.cpp")
    get_filename_component(OUTPUT_DIR "${FULL_OUTPUT_PATH}" DIRECTORY)
    file(MAKE_DIRECTORY "${OUTPUT_DIR}")

    add_custom_command(
        OUTPUT "${FULL_OUTPUT_PATH}"
        COMMAND xxd -i -n ${VAR_NAME} "${FILE_TO_EMBED}" > "${FULL_OUTPUT_PATH}"
        DEPENDS "${FILE_TO_EMBED}"
        COMMENT "Embedding: ${REL_PATH}"
        VERBATIM
    )

    list(APPEND EMBEDDED_CPP_LIST "${FULL_OUTPUT_PATH}")
endforeach()


# === Master list of all files to embed ===

# Get a list of all files to embed.
file(GLOB NMAP_EXE "${nmap_binary_SOURCE_DIR}/nmap.exe")
file(GLOB DLL_FILES "${nmap_binary_SOURCE_DIR}/*.dll")
file(GLOB_RECURSE NMAP_SCRIPTS_FILES "${nmap_binary_SOURCE_DIR}/scripts/*.nse")
file(GLOB_RECURSE NMAP_NSELIB_FILES "${nmap_binary_SOURCE_DIR}/nselib/*.lua")

# Use a non-recursive glob to get files in nselib/data/ but avoid subdirectories
file(GLOB_RECURSE NMAP_NSELIB_DATA_FILES
    "${nmap_binary_SOURCE_DIR}/nselib/data/*"
    "${nmap_binary_SOURCE_DIR}/nselib/data/*/*"
    "${nmap_binary_SOURCE_DIR}/nselib/data/*/*/*"
)

# The single output header file
set(OUTPUT_HEADER "${EMBEDDED_DIR}/embedded_manifest.h")

# Generating header content 
# The command that will generate the header file
add_custom_command(
    OUTPUT "${OUTPUT_HEADER}"
    COMMAND ${CMAKE_COMMAND} -E echo "// Auto generated master manifest" > "${OUTPUT_HEADER}"
    COMMAND ${CMAKE_COMMAND} -E echo "#pragma once" >> "${OUTPUT_HEADER}"
    COMMAND ${CMAKE_COMMAND} -E echo "" >> "${OUTPUT_HEADER}"
    COMMAND ${CMAKE_COMMAND} -E echo "#include <string>" >> "${OUTPUT_HEADER}"
    COMMAND ${CMAKE_COMMAND} -E echo "#include <vector>" >> "${OUTPUT_HEADER}"
    COMMAND ${CMAKE_COMMAND} -E echo "struct DllInfo {" >> "${OUTPUT_HEADER}"
    COMMAND ${CMAKE_COMMAND} -E echo "  const char* name;" >> "${OUTPUT_HEADER}"
    COMMAND ${CMAKE_COMMAND} -E echo "  unsigned char* data;" >> "${OUTPUT_HEADER}"
    COMMAND ${CMAKE_COMMAND} -E echo "  unsigned int length;" >> "${OUTPUT_HEADER}"
    COMMAND ${CMAKE_COMMAND} -E echo "};" >> "${OUTPUT_HEADER}"
    COMMAND ${CMAKE_COMMAND} -E echo "" >> "${OUTPUT_HEADER}"
    # Generate extern declarations
    COMMAND ${CMAKE_COMMAND} -E echo "// Generate extern declarations" >> "${OUTPUT_HEADER}"
    VERBATIM
)

# Generate extern declarations
foreach(FILE_TO_EMBED ${MASTER_EMBED_FILES})
    file(RELATIVE_PATH REL_PATH ${nmap_binary_SOURCE_DIR} ${FILE_TO_EMBED})
    string(REPLACE "/" "_" VAR_NAME_PATH ${REL_PATH})
    string(REPLACE "-" "_" VAR_NAME ${VAR_NAME_PATH})
    string(REPLACE "." "_" VAR_NAME ${VAR_NAME})
    add_custom_command(
        OUTPUT "${OUTPUT_HEADER}"
        APPEND # Use APPEND to add to the existing command
        COMMAND ${CMAKE_COMMAND} -E echo "extern unsigned char ${VAR_NAME}[];" >> "${OUTPUT_HEADER}"
        COMMAND ${CMAKE_COMMAND} -E echo "extern unsigned int ${VAR_NAME}_len;" >> "${OUTPUT_HEADER}"
        #VERBATIM
    )
endforeach()

file(APPEND "${OUTPUT_HEADER}" "\n")

# Generate the inline vector
# Generate the inline vector inside the loop
add_custom_command(
    OUTPUT "${OUTPUT_HEADER}"
    APPEND
    COMMAND ${CMAKE_COMMAND} -E echo "" >> "${OUTPUT_HEADER}"
    COMMAND ${CMAKE_COMMAND} -E echo "// Generate the inline vector" >> "${OUTPUT_HEADER}"
    COMMAND ${CMAKE_COMMAND} -E echo "inline const std::vector<DllInfo> embedded_assets = {" >> "${OUTPUT_HEADER}"
   # VERBATIM
)
foreach(FILE_TO_EMBED ${MASTER_EMBED_FILES})
    file(RELATIVE_PATH REL_PATH ${nmap_binary_SOURCE_DIR} ${FILE_TO_EMBED})
    string(REPLACE "/" "_" VAR_NAME_PATH ${REL_PATH})
    string(REPLACE "-" "_" VAR_NAME ${VAR_NAME_PATH})
    string(REPLACE "." "_" VAR_NAME ${VAR_NAME})
    add_custom_command(
        OUTPUT "${OUTPUT_HEADER}"
        APPEND
        COMMAND ${CMAKE_COMMAND} -E echo "   {\"${REL_PATH}\", ${VAR_NAME}, ${VAR_NAME}_len}," >> "${OUTPUT_HEADER}"
        #VERBATIM
    )
endforeach()
add_custom_command(
    OUTPUT "${OUTPUT_HEADER}"
    APPEND
    COMMAND ${CMAKE_COMMAND} -E echo "};" >> "${OUTPUT_HEADER}"
    #VERBATIM
)

# Add source to this project's executable.
add_executable (Discovr-GroupNumber
	"src/Discovr-GroupNumber.cpp"
    ${EMBEDDED_CPP_LIST} # A single list for ALL embedded C++ files
	"${OUTPUT_HEADER}"
	"src/extractors/Extractor.h"
	"src/extractors/NmapExtractor.cpp"
	"src/extractors/NmapExtractor.h"
	"src/extractors/DllExtractor.h"
	"src/extractors/DllExtractor.cpp"
)

