cmake_minimum_required(VERSION 3.16)

project(ogoAgent VERSION 1.0.0 DESCRIPTION "Native agent to query object graphs" LANGUAGES CXX)
find_program(CLANG_FORMAT_BIN
    NAMES clang-format clang-format-15 clang-format-16
)

if (NOT CLANG_FORMAT_BIN)
    message(WARNING "clang-format not found")
endif()
find_package(antlr4-runtime REQUIRED)
find_package(Java 11 REQUIRED)
find_package(JNI 11 REQUIRED)
if (JNI_FOUND)
    message (STATUS "JNI_INCLUDE_DIRS=${JNI_INCLUDE_DIRS}")
    message (STATUS "JNI_LIBRARIES=${JNI_LIBRARIES}")
endif()


add_library(ogoAgent SHARED "")

add_subdirectory(src)

add_custom_target(format
    ${CLANG_FORMAT_BIN}
    -i
    ${CMAKE_SOURCE_DIR}/src/**/*.cpp
    ${CMAKE_SOURCE_DIR}/src/**/*.h
    COMMENT "Running clang-format"
)

