project(Framework)

add_library(Framework)

target_link_libraries(Framework
  PRIVATE
    SkapeEngine
    ${SKAPE_PLUGINS_LINK}
)

target_sources(Framework
  PRIVATE
    framework/App.cpp

  PUBLIC
    FILE_SET frameworkIncludes
    TYPE HEADERS
    FILES
      framework/App.h
)
