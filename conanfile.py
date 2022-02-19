from conans import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake
from conan.tools.layout import cmake_layout
from conans import tools
from conans.errors import ConanInvalidConfiguration
import os
import re

class PiFan(ConanFile):
   name = "pifan"

   def set_version(self):
      content = tools.load(os.path.join(self.recipe_folder, "CMakeLists.txt"))
      version = re.search(r"project\(PiFan VERSION (.*)\)", content).group(1)
      self.version = version.strip()

   scm = {
       "type": "git",
       "url": "auto",
       "revision": "auto",
   }

   generators = ["CMakeDeps", "CMakeToolchain"]
   settings = "os", "compiler", "build_type", "arch"

   options = {
       "shared": [True, False],
       "fPIC":  [True, False]
   }

   default_options = {
       "shared": False,
       "fPIC": True
   }

   short_paths = True

   def generate(self):
      toolchain = CMakeToolchain(self)
      toolchain.variables["CONAN_EXPORTED"] = True
      toolchain.variables["BUILD_TESTING"] = False

      # There is duplication with the profile
      # https://github.com/conan-io/conan/issues/8922
      if self.settings.arch == "armv8" and self.settings.os == "Linux":
         custom_toolchain = os.path.os.path.join(self.recipe_folder,"CMake","toolchain-RaspberryPi-arm64.cmake")
         toolchain.blocks["user_toolchain"].values["paths"] = [custom_toolchain]

      toolchain.generate()

   def validate(self):
      tools.check_min_cppstd(self, "17")

   def requirements(self):
      # For the moment there is not bcm package and it is being handle in CMake 
      # as an ExternalProject_add
      # self.requires("bcm2835/1.71")
      self.requires("boost/1.78.0#cc6a6b675059b4a70071e8334744502b")
      self.requires("fmt/7.1.3#1ac56aeba28ac0049806e2926b3b5745")
      self.requires("mp-units/0.7.0#7d163303e0c85dd0eeb6085a473d5f8f")
      self.requires("prometheus-cpp/1.0.0#f6c415ec18df8a44cd49d343d837967a")

   def build_requirements(self):
      self.test_requires("gtest/1.11.0#8aca975046f1b60c660ee9d066764d69")

   def imports(self):
      # https://github.com/conan-io/cmake-conan/issues/27
      dest = os.getenv("CONAN_IMPORT_PATH", "")

   def build(self):
      cmake = CMake()
      cmake.configure()
      cmake.build()

   def package(self):
      cmake = CMake()
      cmake.install()
