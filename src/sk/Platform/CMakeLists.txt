
target_sources(SkapeEngine
  PUBLIC
    FILE_SET engineIncludes
    TYPE HEADERS
    FILES
      Platform_Base.h
      Time.h
)

add_subdirectory(Window)