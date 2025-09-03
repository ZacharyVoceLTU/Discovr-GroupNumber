get_filename_component(DLL_HEADER_DIR ${OUTPUT_HEADER} DIRECTORY)
file(MAKE_DIRECTORY ${DLL_HEADER_DIR})

file(GLOB DLL_FILES "${DLL_DIR}/*.dll")

file(WRITE "${OUTPUT_HEADER}" "// Auto generated list of dlls\n")
file(APPEND ${OUTPUT_HEADER} "#pragma once\n\n")
file(APPEND ${OUTPUT_HEADER} "#include <string>\n")
file(APPEND ${OUTPUT_HEADER} "#include <vector>\n")
file(APPEND ${OUTPUT_HEADER} "struct DllInfo {\n")
file(APPEND ${OUTPUT_HEADER} "	   const char* name;\n")
file(APPEND ${OUTPUT_HEADER} "	   unsigned char* data;\n")
file(APPEND ${OUTPUT_HEADER} "	   unsigned int length;\n")
file(APPEND ${OUTPUT_HEADER} "}\n\n")
file(APPEND ${OUTPUT_HEADER} "inline const std::vector<DllInfo> dlls = {\n")

foreach(DLL ${DLL_FILES})
    get_filename_component(DLL_NAME ${DLL} NAME)
    # Convert to corresponding embedded symbol name, e.g., libssl_1_1_dll
    string(REPLACE "." "_" DLL_SYMBOL ${DLL_NAME})
    string(REPLACE "-" "_" DLL_SYMBOL ${DLL_SYMBOL})
    file(APPEND ${OUTPUT_HEADER} "    {\"${DLL_NAME}\", ${DLL_SYMBOL}, ${DLL_SYMBOL}_len},\n")
endforeach()

file(APPEND ${OUTPUT_HEADER} "};\n")