
target_sources(SkapeEngine
  PRIVATE
    Camera.cpp
    CameraFlight.cpp

  PUBLIC
    FILE_SET engineIncludes
    TYPE HEADERS
    FILES
      Camera.h
      CameraFlight.h
)