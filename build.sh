#!/bin/bash

# ----------
# Functions.

function check_deps() {
        # Check dependencies for this project.
        ! hash "mvn" && \
                { echo "missing maven (https://maven.apache.org/download.cgi)"; return 1; }

        ! hash "conan" && \
		{ echo "missing conan (pip install 'conan==2.24.0')"; return 1; }

	java --version | grep '21.' >/dev/null || \
		{ echo "no java 21 available (apt-get install openjdk-21-jdk)"; return 1; }

	conan --version | grep 'version 2.' >/dev/null || \
		{ echo "no conan 2.x available"; return 1; }

        ! hash  "cmake" && \
                { echo "missing cmake (apt-get install cmake)"; return 1; }

        ! hash "clang-format" && \
                { echo "missing clang-format (apt-get install clang-format)"; return 1; }

        ! hash "clang-tidy" && \
                { echo "missing clang-tidy (apt-get install clang-tidy)"; return 1; }

        return 0
}

function compile_ogo() {
        # Build the project.
        ( 
          mvn clean test-compile -Dagent.build.skip=false properties:write-project-properties || \
                  { echo "could not compile ogo"; return 1; }
        )
}

function install_ogo() {
        # Build the project.
        compile_ogo
        ( 
          export LD_LIBRARY_PATH="./src/main/c/build"
          mvn exec:exec@serverStart install -DargLine="-agentlib:ogoAgent"|| \
                  { echo "could not install ogo"; return 1; }
        )
}

function test_ogo() {
        echo "You need to build code first: ${0} compile_ogo"
        ( 
          export LD_LIBRARY_PATH="./src/main/c/build"
          mvn -e exec:exec@serverStart test -DargLine="-agentlib:ogoAgent"
        )
}

function exec_ogo() {
        echo "You need to build code first: ${0} compile_ogo"
        ( 
          export LD_LIBRARY_PATH="./src/main/c/build"
          mvn -e exec:exec@main
        )
}

function format_ogo_java() {
        mvn fmt:format
}

function end_to_end() {
        check_deps || \
                { echo "deps not satisfied"; exit 1; }
        install_ogo || \
                { echo "could not install ogo"; exit 1; } 
}

# ----------
# Main.

check_deps || \
        { echo "No sufficient dependencies"; exit 1; }

"$@"
