
target_sources(SkapeEngine
  PRIVATE
    RenderUtils.cpp
    RenderSurface.cpp

  PUBLIC
    FILE_SET engineIncludes
    TYPE HEADERS
    FILES
      RenderSurface.h
      RenderUtils.h
)