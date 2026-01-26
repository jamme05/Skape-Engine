target_sources(SkapeEngine
  PUBLIC
    FILE_SET engineIncludes
    TYPE HEADERS
    FILES
      Allocator.h
      Map.h
      String.h
      Vector.h
)

add_subdirectory(Const)