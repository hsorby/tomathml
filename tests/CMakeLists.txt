
set(TEST_NAMES
  test_algebraic
  test_odes
)

set(SRCS
  utils.cpp
)

set(HDRS
  strings.h
  utils.h
)

add_library(testhelper STATIC ${SRCS} ${HDRS})
target_link_libraries(testhelper PUBLIC tomathml)
# target_include_directories(testhelper PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

foreach(test_name ${TEST_NAMES})
  add_executable(
    ${test_name}
    ${test_name}.cc
  )

  target_link_libraries(
    ${test_name}
    PUBLIC
    tomathml
    testhelper
    GTest::gtest_main
  )

  gtest_discover_tests(${test_name})

endforeach()
