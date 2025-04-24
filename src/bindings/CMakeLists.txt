include(generatednanobindsources)

get_target_property(_INC_DIRS libtomathml INTERFACE_INCLUDE_DIRECTORIES)

set(GENERATE_SRCS_SCRIPT "${CMAKE_SOURCE_DIR}/src/bindings/doxygen_to_nanobind.py")
set(DOXYGEN_INPUT_SRCS "${CMAKE_SOURCE_DIR}/src/tomathml.h")

generate_nanobind_sources("${GENERATE_SRCS_SCRIPT}" "${DOXYGEN_INPUT_SRCS}" "${CMAKE_CURRENT_BINARY_DIR}/generated/generated_sources.hash")

# Now read the list and register it as sources
file(STRINGS ${GENERATED_FILES_LIST} GENERATED_SOURCES)

foreach(BINDINGS_SOURCE IN LISTS GENERATED_SOURCES)
  get_filename_component(BINDINGS_FILENAME "${BINDINGS_SOURCE}" NAME)  # e.g., "tomathml_bindings.cpp"
  string(REGEX REPLACE "_bindings\\.cpp$" "" BINDINGS_MODULE_NAME "${BINDINGS_FILENAME}")  # remove suffix
  nanobind_add_module(${BINDINGS_MODULE_NAME} ${GENERATED_SOURCES} ${TOMATHML_LIB_SRCS})
  target_include_directories(${BINDINGS_MODULE_NAME} PRIVATE ${_INC_DIRS})
endforeach()
