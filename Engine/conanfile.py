from conans import ConanFile, CMake


class Engine(ConanFile):
    requires = ("glm/0.9.9.8",
                "glad/0.1.34",
                "glfw/3.3.4",
                "libpng/1.6.37",
                "tinyobjloader/1.0.6"
                )
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake"

    def configure(self):
        self.options["glad"].gl_version = "4.6"

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
