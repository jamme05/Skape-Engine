target_sources(SkapeEngine
  PRIVATE
    Asset_Ptr_Base.cpp
    Asset_Raw_Ptr.cpp

  PUBLIC
    FILE_SET engineIncludes
    TYPE HEADERS
    FILES
      Asset_Ptr.h
      Asset_Ptr_Base.h
      Asset_Raw_Ptr.h
      Asset_Ref.h
)