
set(HDRS 
  ${CMAKE_CURRENT_SOURCE_DIR}/cellmltext/parser.h
  ${CMAKE_CURRENT_SOURCE_DIR}/cellmltext/scanner.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/tomathml_export.h
  ${CMAKE_CURRENT_SOURCE_DIR}/tomathml.h
  ${CMAKE_CURRENT_SOURCE_DIR}/utils/stringhelp.h
  ${CMAKE_CURRENT_SOURCE_DIR}/utils/xmllite.h
)
set(SRCS
  ${CMAKE_CURRENT_SOURCE_DIR}/cellmltext/parser.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/cellmltext/scanner.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/tomathml.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/utils/stringhelp.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/utils/xmllite.cpp
)

add_library(libtomathml
  ${SRCS} ${HDRS} 
)
set_target_properties(libtomathml PROPERTIES OUTPUT_NAME "tomathml" POSITION_INDEPENDENT_CODE ON)

# Add include directories
target_include_directories(libtomathml PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}
)

# Show only exported symbols.
set_target_properties(libtomathml PROPERTIES
  CXX_VISIBILITY_PRESET hidden
  VISIBILITY_INLINES_HIDDEN TRUE
  POSITION_INDEPENDENT_CODE ON
)

# Define the export symbol macro
if (BUILD_SHARED_LIBS)
  target_compile_definitions(libtomathml PRIVATE TOMATHML_EXPORTS)
else()
  target_compile_definitions(libtomathml PRIVATE TOMATHML_STATIC)
endif()

# Set compiler warning flags.
if (MSVC)
    target_compile_options(libtomathml PRIVATE /W4)
else()
    target_compile_options(libtomathml PRIVATE -Wall -Wextra -pedantic)
endif()

if(SKBUILD)
  install(TARGETS libtomathml
    COMPONENT wheel
    RUNTIME DESTINATION .
    LIBRARY DESTINATION .
    NAMELINK_SKIP
    EXCLUDE_FROM_ALL
  )
endif()

set(TOMATHML_LIB_SRCS ${SRCS} PARENT_SCOPE)
set(TOMATHML_LIB_HDRS ${HDRS} PARENT_SCOPE)
