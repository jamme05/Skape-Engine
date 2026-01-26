target_sources(SkapeEngine
  PRIVATE
    Asset_List.cpp

  PUBLIC
    FILE_SET engineIncludes
    TYPE HEADERS
    FILES
      Asset_List.h
      Event.h
)