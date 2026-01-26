
target_sources(SkapeEngine
  PRIVATE
    Dynamic_Buffer.cpp
    
  PUBLIC
    FILE_SET engineIncludes
    TYPE HEADERS
    FILES
      Buffer.h
      Dynamic_Buffer.h
      Unsafe_Buffer_Base.h
)