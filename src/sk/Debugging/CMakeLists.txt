target_sources(SkapeEngine
  PUBLIC
    FILE_SET engineIncludes
    TYPE HEADERS
    FILES
      Debugging.h
      Severity.h
)

add_subdirectory(Macros)