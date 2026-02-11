
target_sources(SkapeEngine
  PRIVATE
    Print.cpp
    StringID.cpp
    UUID.cpp

  PUBLIC
    FILE_SET engineIncludes
    TYPE HEADERS
    FILES
      Concepts.h
      Counter.h
      DerivedSingleton.h
      Hashing.h
      Offsetof.h
      Print.h
      Singleton.h
      Smart_Ptrs.h
      StringID.h
      UUID.h
      Visitor.h
)