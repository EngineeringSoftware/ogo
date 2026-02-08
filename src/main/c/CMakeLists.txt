cmake_minimum_required(VERSION 3.16)

project(ogoAgent VERSION 1.0.0 DESCRIPTION "Native agent to query object graphs" LANGUAGES CXX)

# ------------------------------------------------------------
# Dependencies
# ------------------------------------------------------------
find_package(antlr4-runtime REQUIRED)
find_package(Java 11 REQUIRED)
find_package(JNI 11 REQUIRED)
if (JNI_FOUND)
    message (STATUS "JNI_INCLUDE_DIRS=${JNI_INCLUDE_DIRS}")
    message (STATUS "JNI_LIBRARIES=${JNI_LIBRARIES}")
endif()

# ------------------------------------------------------------
# ANTLR Code Generation
# ------------------------------------------------------------
set(ANTLR_VERSION "4.12.0")
set(ANTLR_JAR_DIR "${CMAKE_SOURCE_DIR}/.cache")
set(ANTLR_JAR "${ANTLR_JAR_DIR}/antlr-${ANTLR_VERSION}-complete.jar")
set(ANTLR_GRAMMAR "${CMAKE_SOURCE_DIR}/../resources/Cypher.g4")
set(ANTLR_OUTPUT_DIR "${CMAKE_SOURCE_DIR}/generated")

# Download ANTLR jar if not present (matches the C++ runtime version from Conan)
if(NOT EXISTS "${ANTLR_JAR}")
    message(STATUS "Downloading ANTLR ${ANTLR_VERSION} JAR...")
    file(MAKE_DIRECTORY "${ANTLR_JAR_DIR}")
    file(DOWNLOAD 
        "https://www.antlr.org/download/antlr-${ANTLR_VERSION}-complete.jar"
        "${ANTLR_JAR}"
        SHOW_PROGRESS
        STATUS DOWNLOAD_STATUS
    )
    list(GET DOWNLOAD_STATUS 0 STATUS_CODE)
    if(NOT STATUS_CODE EQUAL 0)
        message(FATAL_ERROR "Failed to download ANTLR jar")
    endif()
endif()

# List all files that ANTLR will generate from the grammar
set(ANTLR_GENERATED_FILES
    "${ANTLR_OUTPUT_DIR}/CypherLexer.cpp"
    "${ANTLR_OUTPUT_DIR}/CypherLexer.h"
    "${ANTLR_OUTPUT_DIR}/CypherParser.cpp"
    "${ANTLR_OUTPUT_DIR}/CypherParser.h"
    "${ANTLR_OUTPUT_DIR}/CypherBaseVisitor.cpp"
    "${ANTLR_OUTPUT_DIR}/CypherBaseVisitor.h"
    "${ANTLR_OUTPUT_DIR}/CypherVisitor.cpp"
    "${ANTLR_OUTPUT_DIR}/CypherVisitor.h"
)

# Define custom command to generate C++ files from ANTLR grammar
add_custom_command(
    OUTPUT ${ANTLR_GENERATED_FILES}
    COMMAND ${CMAKE_COMMAND} -E make_directory "${ANTLR_OUTPUT_DIR}"
    COMMAND ${Java_JAVA_EXECUTABLE} -jar "${ANTLR_JAR}"
        -Dlanguage=Cpp
        -visitor
        -no-listener
        -Xexact-output-dir
        -o "${ANTLR_OUTPUT_DIR}"
        "${ANTLR_GRAMMAR}"
    DEPENDS "${ANTLR_GRAMMAR}"
    COMMENT "Generating ANTLR4 C++ files from grammar..."
    VERBATIM
)

# Create a custom target that depends on the generated files
add_custom_target(generate_antlr DEPENDS ${ANTLR_GENERATED_FILES})

# ------------------------------------------------------------
# Find clang-format
# ------------------------------------------------------------
set(OGO_CLANG_SEARCH_PATH
    "/usr/local/bin"
    "/usr/bin"
    "/usr/local/opt/llvm/bin"
    "/usr/local/opt/llvm@15/bin"
    "/opt/homebrew/opt/llvm@15/bin"
)

find_program(CLANG_FORMAT_BIN
    NAMES clang-format clang-format-15 clang-format-16
    HINTS ${OGO_CLANG_SEARCH_PATH}
)

if(NOT CLANG_FORMAT_BIN)
    message(WARNING "clang-format not found. 'make format' will fail.")
else()
    message(STATUS "Found clang-format at ${CLANG_FORMAT_BIN}")
endif()


# ------------------------------------------------------------
# Build
# ------------------------------------------------------------
add_library(ogoAgent SHARED "")
add_dependencies(ogoAgent generate_antlr)
add_subdirectory(src)


# ------------------------------------------------------------
# Format target
# ------------------------------------------------------------
if(CLANG_FORMAT_BIN)
    # Collect all source files
    file(GLOB_RECURSE ALL_SOURCE_FILES
        ${CMAKE_SOURCE_DIR}/src/*.cpp
        ${CMAKE_SOURCE_DIR}/src/*.h
    )
    
    add_custom_target(format
        COMMAND ${CLANG_FORMAT_BIN}
        -i
        ${ALL_SOURCE_FILES}
        COMMENT "Running clang-format"
    )
endif()

