
find_package(Doxygen)
find_package(Python 3.8
  REQUIRED COMPONENTS Interpreter Development.Module
  OPTIONAL_COMPONENTS Development.SABIModule
)

if (Python_FOUND)
  # Detect the installed nanobind package and import it into CMake
  execute_process(
    COMMAND "${Python_EXECUTABLE}" -m nanobind --cmake_dir
    OUTPUT_STRIP_TRAILING_WHITESPACE OUTPUT_VARIABLE nanobind_ROOT)
  find_package(nanobind CONFIG)
endif()

if (Python_FOUND AND DOXYGEN_FOUND AND nanobind_FOUND)
  set(BINDINGS_AVAILABLE TRUE)
endif()
