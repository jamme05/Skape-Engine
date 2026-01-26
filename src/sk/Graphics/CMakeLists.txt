
target_sources(SkapeEngine
  PRIVATE
    Renderer.cpp
    
  PUBLIC
    FILE_SET engineIncludes
    TYPE HEADERS
    FILES
      Renderer.h
)

add_subdirectory(Buffer)
add_subdirectory(Passes)
add_subdirectory(Pipelines)
add_subdirectory(Utils)
