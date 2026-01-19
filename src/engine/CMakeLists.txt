project(SkapeEngine)

add_library(SkapeEngine)

target_sources(SkapeEngine
  PRIVATE
    Assets/Asset.cpp

  PUBLIC
    FILE_SET engineIncludes
    TYPE HEADERS
    FILES
      Skape_Main.h
)
