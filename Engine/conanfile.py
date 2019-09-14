from conans import ConanFile, CMake

class Engine(ConanFile):
    requires = ("glm/0.9.9.1@g-truc/stable",
                "glad/0.1.29@bincrafters/stable",
                "glfw/3.3@bincrafters/stable",
                "libpng/1.6.36@bincrafters/stable",
                )
    generators = "cmake"

    def configure(self):
        self.options["glad"].api_version = "4.6"

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
