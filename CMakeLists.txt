# CMakeList.txt : CMake project for Discovr-GroupNumber, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.20)
project ("Discovr-GroupNumber" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

include(FetchContent)

set(EMBEDDED_DIR ${CMAKE_SOURCE_DIR}/src/NmapEmbedded)
file(MAKE_DIRECTORY "${EMBEDDED_DIR}")

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message(STATUS "Configuring for Windows...")

    #  === Download Nmap For Windows ===
    FetchContent_Declare(
        nmap_binary
        URL https://nmap.org/dist/nmap-7.92-win32.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    FetchContent_MakeAvailable(nmap_binary)

    # === Master list of all files to embed ===
    

    # Get a list of all files to embed. Be specific to avoid directories and unwanted files.
    file(GLOB_RECURSE NMAP_BINARY "${nmap_binary_SOURCE_DIR}/*")

    # Combine all lists into a single master list
    set(MASTER_EMBED_FILES ${NMAP_BINARY})

    set(EMBEDDED_CPP_LIST "")

    # === Single loop to process all files ===
    foreach(FILE_TO_EMBED ${MASTER_EMBED_FILES})
        get_filename_component(FILENAME ${FILE_TO_EMBED} NAME)

        if (FILENAME MATCHES "^[0-9]")
            MESSAGE(STATUS "Skipping file starting with a digit: ${FILENAME}")
            continue()
        endif()

        file(RELATIVE_PATH REL_PATH ${nmap_binary_SOURCE_DIR} ${FILE_TO_EMBED})
        string(REPLACE "/" "__" VAR_NAME_PATH ${REL_PATH})
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

    # The single output header file
    set(OUTPUT_HEADER "${EMBEDDED_DIR}/embedded_manifest.h")
    set(DEPENDS_ON_FILES "") # We'll populate this later

    # WHY DOES THIS KEEP BREAKING EVEN THOUGH I DONT TOUCH IT!!!

    # Build the content of the header file as a single string
    set(HEADER_CONTENT_STRING "// Auto generated master manifest
    #pragma once

    #include <vector>
    struct DllInfo {
    const char* name;
    unsigned char* data;
    unsigned int length;
    };

    // Generate extern declarations
    ")

    # Add extern declarations
    foreach(FILE_TO_EMBED ${MASTER_EMBED_FILES})
        get_filename_component(FILENAME ${FILE_TO_EMBED} NAME)

        if (FILENAME MATCHES "^[0-9]")
            MESSAGE(STATUS "Skipping file starting with a digit: ${FILENAME}")
            continue()
        endif()

        file(RELATIVE_PATH REL_PATH ${nmap_binary_SOURCE_DIR} ${FILE_TO_EMBED})
        string(REPLACE "/" "__" VAR_NAME_PATH ${REL_PATH})
        string(REPLACE "-" "_" VAR_NAME ${VAR_NAME_PATH})
        string(REPLACE "." "_" VAR_NAME ${VAR_NAME})
        string(APPEND HEADER_CONTENT_STRING "extern unsigned char ${VAR_NAME}[];\n")
        string(APPEND HEADER_CONTENT_STRING "extern unsigned int ${VAR_NAME}_len;\n")
    endforeach()

    string(APPEND HEADER_CONTENT_STRING "
    // Generate the inline vector
    inline const std::vector<DllInfo> embedded_assets = {
    ")

    # Add the inline vector content
    foreach(FILE_TO_EMBED ${MASTER_EMBED_FILES})
        get_filename_component(FILENAME ${FILE_TO_EMBED} NAME)

        if (FILENAME MATCHES "^[0-9]")
            MESSAGE(STATUS "Skipping file starting with a digit: ${FILENAME}")
            continue()
        endif()

        file(RELATIVE_PATH REL_PATH ${nmap_binary_SOURCE_DIR} ${FILE_TO_EMBED})
        string(REPLACE "/" "__" VAR_NAME_PATH ${REL_PATH})
        string(REPLACE "-" "_" VAR_NAME ${VAR_NAME_PATH})
        string(REPLACE "." "_" VAR_NAME ${VAR_NAME})
        string(APPEND HEADER_CONTENT_STRING "   {\"${REL_PATH}\", ${VAR_NAME}, ${VAR_NAME}_len},\n")
    endforeach()

    string(APPEND HEADER_CONTENT_STRING "};")

    # Create a temporary file to hold the generated content
    set(TEMP_HEADER_PATH "${CMAKE_BINARY_DIR}/embedded_manifest.h.tmp")
    file(WRITE "${TEMP_HEADER_PATH}" "${HEADER_CONTENT_STRING}")

    # Add a custom command to copy the temporary file to the final destination
    # The DEPENDS here ensures the command is triggered whenever the input files change.
    add_custom_command(
        OUTPUT "${OUTPUT_HEADER}"
        COMMAND ${CMAKE_COMMAND} -E copy "${TEMP_HEADER_PATH}" "${OUTPUT_HEADER}"
        DEPENDS ${MASTER_EMBED_FILES}
        COMMENT "Generating embedded_manifest.h"
    )

    # --- END: The single, correct block to generate the header ---

elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(STATUS "Configuring for Linux")

    FetchContent_Declare(
        nmap_static
        URL https://github.com/ZacharyVoceLTU/Discovr-GroupNumber-Binaries/releases/download/pre-release0.8/nmap_static.tar.gz
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    FetchContent_MakeAvailable(nmap_static)

    set(NMAP_STATIC_DIR "${nmap_static_SOURCE_DIR}/nmap")

    file(GLOB NMAP_STATIC "${nmap_static_SOURCE_DIR}/*")

    set(MASTER_EMBED_FILES ${NMAP_STATIC})

    get_filename_component(FILE_NAME ${NMAP_STATIC_DIR} NAME)
    file(RELATIVE_PATH REL_PATH ${nmap_static_SOURCE_DIR} ${NMAP_STATIC_DIR})
    set(FULL_OUTPUT_PATH "${EMBEDDED_DIR}/${REL_PATH}_embedded.cpp")

    add_custom_command(
        OUTPUT "${FULL_OUTPUT_PATH}"
        COMMAND xxd -i -n ${REL_PATH} "${nmap_static_SOURCE_DIR}" > "${FULL_OUTPUT_PATH}"
        DEPENDS "${NMAP_STATIC_DIR}"
        COMMENT "Embedding: ${REL_PATH}"
        VERBATIM
    )

     set(OUTPUT_HEADER "${EMBEDDED_DIR}/embedded_manifest.h")
     set(TEMP_HEADER_PATH "${CMAKE_BINARY_DIR}/embedded_manifest.h.tmp")
     file(WRITE "${TEMP_HEADER_PATH}" "Hello")

     set(TEMP "Hello")

     add_custom_command(
        OUTPUT "${OUTPUT_HEADER}"
        COMMAND ${CMAKE_COMMAND} -E copy ${TEMP_HEADER_PATH} "${OUTPUT_HEADER}"
        DEPENDS ${MASTER_EMBED_FILES}
        COMMENT "Generating embedded_manifest.h"
     )

endif()


# Add source to this project's executable.
add_executable (Discovr-GroupNumber
    "src/Discovr-GroupNumber.cpp"
    ${EMBEDDED_CPP_LIST} # A single list for ALL embedded C++ files
    "${OUTPUT_HEADER}" # I know bad practice but it works for now
    "src/extractors/Extractor.cpp"
)


# Mark the generated files as generated so CMake knows they don't exist yet
set_source_files_properties(${EMBEDDED_CPP_LIST} PROPERTIES GENERATED TRUE)
set_source_files_properties("${OUTPUT_HEADER}" PROPERTIES GENERATED TRUE)


# Create a custom target that generates all embedded files
add_custom_target(generate_embedded_files
    DEPENDS ${EMBEDDED_CPP_LIST} "${OUTPUT_HEADER}"
    COMMENT "Generating all embedded files"
)

# Make sure the executable depends on the generation target
add_dependencies(Discovr-GroupNumber generate_embedded_files)
