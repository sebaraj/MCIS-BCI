from conan import ConanFile


class MCISConan(ConanFile):
    name = "MCIS-Algo_Test"
    version = "0.1"
    settings = "os", "compiler", "build_type", "arch"
    requires = [
        "gtest/1.17.0",
        "llvm-openmp/20.1.6",
    ]
    generators = "CMakeDeps", "CMakeToolchain"

    def configure(self):
        self.options["gtest"].shared = False
        self.settings.compiler.cppstd = "23"
