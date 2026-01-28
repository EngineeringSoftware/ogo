cmake_minimum_required(VERSION 3.16)

project(ogoAgent VERSION 1.0.0 DESCRIPTION "Native agent to query object graphs" LANGUAGES CXX)


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
# Build
# ------------------------------------------------------------
add_library(ogoAgent SHARED "")
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


# ------------------------------------------------------------
# clang-tidy
# ------------------------------------------------------------
find_program(CLANG_TIDY_BIN
    NAMES clang-tidy clang-tidy-15 clang-tidy-16
    HINTS ${OGO_CLANG_SEARCH_PATH}
)


if(NOT CLANG_TIDY_BIN)
    message(WARNING "clang-tidy not found; check-lint and fix-lint disabled. Install with: apt-get install clang-tidy")
else()
    message(STATUS "Found clang-tidy at ${CLANG_TIDY_BIN}")
   
    file(GLOB_RECURSE PROJECT_LINT_FILES
        "${CMAKE_SOURCE_DIR}/src/*.h"
        "${CMAKE_SOURCE_DIR}/src/*.cpp"
    )


    add_custom_target(check-lint
        COMMAND ${CLANG_TIDY_BIN}
            -p ${CMAKE_BINARY_DIR}
            ${PROJECT_LINT_FILES}
        COMMENT "Running clang-tidy"
    )
   
    add_custom_target(fix-lint
        COMMAND ${CLANG_TIDY_BIN}
            -p ${CMAKE_BINARY_DIR}
            --fix
            ${PROJECT_LINT_FILES}
        COMMENT "Running clang-tidy with auto-fix"
    )
endif()