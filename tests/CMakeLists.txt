
set(TEST_NAMES
  test_algebraic
  test_odes
)

# Not actually used because the testhelper library is an interface library.
set(HDRS
  strings.h
)

add_library(testhelper INTERFACE)
target_include_directories(testhelper INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

foreach(test_name ${TEST_NAMES})
  add_executable(
    ${test_name}
    ${test_name}.cc
  )

  target_link_libraries(
    ${test_name}
    PUBLIC
    libtomathml
    testhelper
    GTest::gtest_main
  )

  gtest_discover_tests(${test_name})

endforeach()
