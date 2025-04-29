
set(TEST_NAMES
  test_algebraic
  test_odes
)

# Not actually used because the testhelper library is an interface library.
set(HDRS
  expectedresultstrings.h
)

add_library(testhelper INTERFACE)
target_include_directories(testhelper INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

if(MSVC)
  set(MSVC_TEST_LAUNCHER_EXTENSION "${CMAKE_COMMAND}" -E env --modify "PATH=path_list_prepend:$<TARGET_FILE_DIR:gtest>" --modify "PATH=path_list_prepend:$<TARGET_FILE_DIR:libtomathml>")
endif()

foreach(test_name ${TEST_NAMES})
  add_executable(
    ${test_name}
    ${test_name}.cc
  )

  if (MSVC)
    set_target_properties(${test_name} PROPERTIES
      TEST_LAUNCHER "${MSVC_TEST_LAUNCHER_EXTENSION}"
    )
  endif()

  target_link_libraries(
    ${test_name}
    PUBLIC
    libtomathml
    testhelper
    GTest::gtest_main
  )

  gtest_discover_tests(${test_name})

endforeach()
