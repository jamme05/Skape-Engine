project(Framework)

add_library(Framework STATIC)

target_link_libraries(Framework
  PUBLIC
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
