include(generatednanobindsources)

set(GENERATE_SRCS_SCRIPT "${CMAKE_SOURCE_DIR}/src/bindings/doxygen_to_nanobind.py")
set(DOXYGEN_INPUT_SRCS "${CMAKE_SOURCE_DIR}/src/tomathml.h")

generate_nanobind_sources("${GENERATE_SRCS_SCRIPT}" "${DOXYGEN_INPUT_SRCS}" "${CMAKE_CURRENT_BINARY_DIR}/generated/generated_sources.hash")

# Read the source file list and register it as sources.
file(STRINGS ${GENERATED_FILES_LIST} GENERATED_SOURCES)

foreach(BINDINGS_SOURCE IN LISTS GENERATED_SOURCES)
  get_filename_component(BINDINGS_FILENAME "${BINDINGS_SOURCE}" NAME)
  string(REGEX REPLACE "_bindings\\.cpp$" "" BINDINGS_MODULE_NAME "${BINDINGS_FILENAME}")  # Remove suffix.
  nanobind_add_module(${BINDINGS_MODULE_NAME} ${GENERATED_SOURCES})
  target_link_libraries(${BINDINGS_MODULE_NAME} PUBLIC libtomathml)
endforeach()
