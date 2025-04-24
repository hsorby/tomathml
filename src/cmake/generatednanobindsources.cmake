# Requires Doxygen to be found and available and also a Python environment with
# nanobind installed.
# Sets the variable GENERATED_FILES_LIST in the parent scope to specify where the file
# containing the list of generated files.

function(generate_nanobind_sources SCRIPT INPUTS HASH_FILE)
  # Combine all hashes
  set(GEN_SCRIPT_HASH "")
  file(SHA256 "${SCRIPT}" SCRIPT_HASH)
  set(GEN_SCRIPT_HASH "${SCRIPT_HASH}")

  foreach(INPUT_FILE IN LISTS INPUTS)
    file(SHA256 "${INPUT_FILE}" FILE_HASH)
    set(GEN_SCRIPT_HASH "${GEN_SCRIPT_HASH}${file_hash}")
  endforeach()

  # Create a temp hash file to compare
  set(HASH_TEMP "${HASH_FILE}.tmp")
  file(WRITE "${HASH_TEMP}" "${GEN_SCRIPT_HASH}")

  # Compare with previous hash
  set(OLD_HASH "")
  if(EXISTS "${HASH_FILE}")
    file(READ "${HASH_FILE}" OLD_HASH)
  endif()

  set(GENERATED_DIR ${CMAKE_BINARY_DIR}/src/bindings/generated)
  set(GENERATED_FILES_LIST ${GENERATED_DIR}/generated_files.txt PARENT_SCOPE)

  if(NOT OLD_HASH STREQUAL GEN_SCRIPT_HASH)
    message(STATUS "Changes detected — regenerating binding source files: ${SCRIPT}")

    # Configure Doxygen config file from template
    set(DOXYGEN_INPUT_SRCS "${CMAKE_SOURCE_DIR}/src/tomathml.h")

    set(DOXYGEN_OUTPUT_DIR "${CMAKE_BINARY_DIR}/docs/doxygen")
    set(DOXYGEN_XML_DIR "${DOXYGEN_OUTPUT_DIR}/xml")


    configure_file(${CMAKE_SOURCE_DIR}/src/cmake/doxyfile.in.config ${DOXYGEN_OUTPUT_DIR}/Doxyfile @ONLY)

    execute_process(
      COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUTPUT_DIR}/Doxyfile
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      OUTPUT_QUIET
      RESULT_VARIABLE RESULT
    )

    if(NOT RESULT EQUAL 0)
      message(FATAL_ERROR "Doxygen generation failed: ${DOXYGEN_EXECUTABLE}")
    endif()

    execute_process(
      COMMAND ${CMAKE_COMMAND} -E make_directory ${GENERATED_DIR}
      COMMAND ${Python_EXECUTABLE} "${SCRIPT}"
              --xml ${DOXYGEN_XML_DIR}
              --out ${GENERATED_DIR}
      OUTPUT_QUIET
      RESULT_VARIABLE RESULT
    )

    if(NOT RESULT EQUAL 0)
      message(FATAL_ERROR "Generator script failed: ${SCRIPT}")
    endif()

    file(RENAME "${HASH_TEMP}" "${HASH_FILE}")
  else()
    file(REMOVE "${HASH_TEMP}")
    message(STATUS "No changes — skipping bindings source generation.")
  endif()
endfunction()

