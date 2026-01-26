
target_sources(SkapeEngine
  PRIVATE
    ClassTest.cpp
    Types.cpp

  PUBLIC
    FILE_SET engineIncludes
    TYPE HEADERS
    FILES
      ClassMacros.h
      Enum.h
      Global_Registry.h
      Interface.h
      RuntimeClass.h
      RuntimeStruct.h
      Type_Hash.h
      Type_Registry.h
      Types.h
)

add_subdirectory(Manager)