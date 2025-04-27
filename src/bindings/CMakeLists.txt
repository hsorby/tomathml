include(generatednanobindsources)

set(GENERATE_SRCS_SCRIPT "${CMAKE_SOURCE_DIR}/src/bindings/doxygen_to_nanobind.py")
set(DOXYGEN_INPUT_SRCS "${CMAKE_SOURCE_DIR}/src/tomathml.h")
set(HASH_FILE "${CMAKE_CURRENT_BINARY_DIR}/generated/generated_sources.hash")

generate_nanobind_sources("${GENERATE_SRCS_SCRIPT}" "${DOXYGEN_INPUT_SRCS}" "${HASH_FILE}")

# Read the source file list and register it as sources.
file(STRINGS ${GENERATED_FILES_LIST} GENERATED_SOURCES)

if(APPLE)
  set(RPATH_ORIGIN "@loader_path/.")
elseif(UNIX)
  set(RPATH_ORIGIN "\$ORIGIN")
else()
  set(RPATH_ORIGIN "no_rpath")
endif()

# ${CMAKE_COMMAND} -E env --modify PATH PREPEND "${MY_DLL_DIR}"

set(BINDINGS_TARGETS)
foreach(BINDINGS_SOURCE IN LISTS GENERATED_SOURCES)
  get_filename_component(BINDINGS_FILENAME "${BINDINGS_SOURCE}" NAME)
  string(REGEX REPLACE "_bindings\\.cpp$" "" BINDINGS_MODULE_NAME "${BINDINGS_FILENAME}")
  nanobind_add_module(${BINDINGS_MODULE_NAME} STABLE_ABI ${GENERATED_SOURCES})
  target_link_libraries(${BINDINGS_MODULE_NAME} PUBLIC libtomathml)
  set_target_properties(${BINDINGS_MODULE_NAME} PROPERTIES
    BUILD_WITH_INSTALL_RPATH TRUE
    INSTALL_RPATH "${RPATH_ORIGIN}"
  )
  list(APPEND BINDINGS_TARGETS ${BINDINGS_MODULE_NAME})
endforeach()

if(SKBUILD)
  install(TARGETS ${BINDINGS_TARGETS}
    COMPONENT wheel
    DESTINATION .
    EXCLUDE_FROM_ALL
  )
endif()
