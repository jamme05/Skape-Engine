target_sources(SkapeEngine
  PRIVATE
    Asset_Job_Manager.cpp
    Asset_Manager.cpp

  PUBLIC
    FILE_SET engineIncludes
    TYPE HEADERS
    FILES
      Asset_Job_Manager.h
      Asset_Manager.h
)