#!/bin/bash
# OGO Development Tasks
# Usage: ./tasks.sh [command]

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

        echo "deps ok"

        return 0
}

function install_deps() {
        echo "Installing dependencies..."

        if ! hash "mvn" 2>/dev/null; then
                echo "Installing Maven..."
                apt-get update && apt-get install -y maven || \
                        { echo "Failed to install Maven"; return 1; }
        fi

        if ! hash "java" 2>/dev/null || ! java --version 2>&1 | grep -q '21.'; then
                echo "Installing Java 21..."
                apt-get update && apt-get install -y openjdk-21-jdk || \
                        { echo "Failed to install Java 21"; return 1; }
        fi

        if ! hash "conan" 2>/dev/null; then
                echo "Installing Conan..."
                pip install 'conan==2.24.0' || \
                        { echo "Failed to install Conan"; return 1; }
        fi

        if ! hash "cmake" 2>/dev/null; then
                echo "Installing CMake..."
                apt-get update && apt-get install -y cmake || \
                        { echo "Failed to install CMake"; return 1; }
        fi

        if ! hash "clang-format" 2>/dev/null; then
                echo "Installing Clang Format..."
                apt-get update && apt-get install -y clang-format || \
                        { echo "Failed to install Clang Format"; return 1; }
        fi

        echo "Dependencies installed successfully!"
        return 0
}

function compile() {
        # Build the project.
        ( 
          mvn clean compile -Dagent.build.skip=false properties:write-project-properties || \
                  { echo "could not compile ogo"; return 1; }
        )
}

function install() {
        # Build the project.
        compile
        ( 
          export LD_LIBRARY_PATH="./src/main/c/build"
          export JAVA_TOOL_OPTIONS="-Djava.util.logging.config.file=./src/main/resources/logging.properties"
          mvn exec:exec@serverStart install -DargLine="-agentlib:ogoAgent"||
                  { echo "could not install ogo"; return 1; }
        )
}

function tests() {
        check_deps || \
                { echo "Dependencies not satisfied. Please install with: ./tasks.sh install_deps"; return 1; }
        echo "Please make sure you have ran ./tasks.sh compile"
        ( 
          export LD_LIBRARY_PATH="./src/main/c/build"
          export JAVA_TOOL_OPTIONS="-Djdk.attach.allowAttachSelf=true -Djava.security.manager=allow -Djava.util.logging.config.file=./src/main/resources/logging.properties"
          mvn -e exec:exec@serverStart test -DargLine="-agentlib:ogoAgent"
        )
}

function exec() {
        check_deps || \
                { echo "Dependencies not satisfied. Please install with: ./tasks.sh install_deps"; return 1; }
        echo "Please make sure you have ran ./tasks.sh compile"
        ( 
          export LD_LIBRARY_PATH="./src/main/c/build"
          export JAVA_TOOL_OPTIONS="-Djava.util.logging.config.file=./src/main/resources/logging.properties"
          mvn exec:exec@serverStart && sleep 4 && mvn -e exec:exec@main
        )
}

function format() {
        mvn formatter:format || \
                { echo "could not format java sources"; return 1; }

        if [ -d "./src/main/c/build" ]; then
                cmake --build ./src/main/c/build --target format || \
                        { echo "could not format c++ sources"; return 1; }
        else
                echo "skipping c++ format: ./src/main/c/build not found (run ./tasks.sh compile first)"
        fi
}

function end_to_end() {
        check_deps || \
                { echo "deps not satisfied"; exit 1; }
        install || \
                { echo "could not install ogo"; exit 1; } 
}

"$@"
