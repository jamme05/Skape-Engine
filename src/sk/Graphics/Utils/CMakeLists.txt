
target_sources(SkapeEngine
  PRIVATE
    RenderUtils.cpp

  PUBLIC
    FILE_SET engineIncludes
    TYPE HEADERS
    FILES
      RenderUtils.h
)