
set(HDRS 
  cellmltext/parser.h
  cellmltext/scanner.h
  include/tomathml_export.h
  tomathml.h
  utils/stringhelp.h
  utils/xmllite.h
)
set(SRCS
  cellmltext/parser.cpp
  cellmltext/scanner.cpp
  tomathml.cpp
  utils/stringhelp.cpp
  utils/xmllite.cpp
)

add_library(tomathml 
  ${SRCS} ${HDRS} 
)

# Add include directories
target_include_directories(tomathml PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}
)

# Show only exported symbols.
set_target_properties(tomathml PROPERTIES
  CXX_VISIBILITY_PRESET hidden
  VISIBILITY_INLINES_HIDDEN TRUE
)

# Define the export symbol macro
if (BUILD_SHARED_LIBS)
  target_compile_definitions(tomathml PRIVATE TOMATHML_EXPORTS)
else()
  target_compile_definitions(tomathml PRIVATE TOMATHML_STATIC)
endif()

# Set compiler warning flags.
if (MSVC)
    target_compile_options(tomathml PRIVATE /W4)
else()
    target_compile_options(tomathml PRIVATE -Wall -Wextra -pedantic)
endif()
