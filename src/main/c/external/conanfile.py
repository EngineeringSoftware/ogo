from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMakeDeps
from conan.tools.env import VirtualBuildEnv, VirtualRunEnv

class AppConan(ConanFile):
    settings = "os", "arch", "compiler", "build_type"

    requires = "antlr4-cppruntime/4.12.0"

    default_options = {"antlr4-cppruntime/*:shared": True}

    def generate(self):
        CMakeToolchain(self).generate()
        CMakeDeps(self).generate()
        VirtualBuildEnv(self).generate()
        VirtualRunEnv(self).generate()
