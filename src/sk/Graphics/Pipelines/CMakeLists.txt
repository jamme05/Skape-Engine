
target_sources(SkapeEngine
  PRIVATE
    Deferred_Pipeline.cpp
    Forward_Pipeline.cpp
    Pipeline.cpp

  PUBLIC
    FILE_SET engineIncludes
    TYPE HEADERS
    FILES
      Deferred_Pipeline.h
      Forward_Pipeline.h
      Pipeline.h
)