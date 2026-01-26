
target_sources(SkapeEngine
  PRIVATE
    Type_Manager.cpp

  PUBLIC
    FILE_SET engineIncludes
    TYPE HEADERS
    FILES
      Type_Manager.h
)