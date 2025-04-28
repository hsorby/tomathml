
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
  set(MSVC_TEST_EXECUTOR_EXTENSION PROPERTIES TEST_EXECUTOR "\"${CMAKE_COMMAND}\" -E env --modify PATH=path_list_prepend:$<TARGET_FILE_DIR:gtest> --modify PATH=path_list_prepend:$<SHELL_PATH:$<TARGET_FILE_DIR:libtomathml>>")
  set(MSVC_TEST_LAUNCHER_EXTENSION "\"${CMAKE_COMMAND}\" -E env --modify PATH=path_list_prepend:$<TARGET_FILE_DIR:gtest> --modify PATH=path_list_prepend:$<TARGET_FILE_DIR:libtomathml> --")
  if(SKBUILD)
    set(MSVC_SK_BUILD_OPTION DISCOVERY_MODE PRE_TEST)
  endif()
endif()


message(STATUS "==================")
message(STATUS "MSVC_TEST_EXECUTOR_EXTENSION: ${MSVC_TEST_EXECUTOR_EXTENSION}")
message(STATUS "MSVC_TEST_LAUNCHER_EXTENSION: ${MSVC_TEST_LAUNCHER_EXTENSION}")

foreach(test_name ${TEST_NAMES})
  add_executable(
    ${test_name}
    ${test_name}.cc
  )

  if (MSVC)
    set_target_properties(${test_name} PROPERTIES
      TEST_LAUNCHER ${MSVC_TEST_LAUNCHER_EXTENSION}
    )
  endif()

  target_link_libraries(
    ${test_name}
    PUBLIC
    libtomathml
    testhelper
    GTest::gtest_main
  )

  gtest_discover_tests(${test_name}
  )

endforeach()
