# List source files in dir
file(GLOB ogo_SOURCES CONFIGURE_DEPENDS "*.h" "*.cpp")
file(GLOB ogo_GENERATED_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/../generated/*.h" "${CMAKE_CURRENT_LIST_DIR}/../generated/*.cpp")

target_sources(ogoAgent PUBLIC ${ogo_SOURCES} ${ogo_GENERATED_SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE antlr4_shared)

# Add current dir to include header path
target_include_directories(ogoAgent PUBLIC ${CMAKE_CURRENT_LIST_DIR})

# Add generated ANTLR headers to include path
target_include_directories(ogoAgent PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../generated)

# Add JNI dir to include header path
target_include_directories(ogoAgent PUBLIC ${JNI_INCLUDE_DIRS})

# c++17 required for supporting std::any 
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
