target_sources(SkapeEngine
  PRIVATE
    Asset_Loader.cpp

  PUBLIC
    FILE_SET engineIncludes
    TYPE HEADERS
    FILES
      Asset_Loader.h
      WorkerTask.h
)