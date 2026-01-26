
target_sources(SkapeEngine
  PRIVATE
    Seralization.cpp
    Serializable.cpp
    SerializedObject.cpp

  PUBLIC
    FILE_SET engineIncludes
    TYPE HEADERS
    FILES
      Seralization.h
      Serializable.h
      SerializedObject.h
)