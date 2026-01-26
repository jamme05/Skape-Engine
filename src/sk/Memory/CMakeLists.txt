
target_sources(SkapeEngine
  PUBLIC
    FILE_SET engineIncludes
    TYPE HEADERS
    FILES
      Memory.h
)

add_subdirectory(Tracker)