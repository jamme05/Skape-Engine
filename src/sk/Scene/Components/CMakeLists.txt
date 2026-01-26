
target_sources(SkapeEngine
  PRIVATE
    CameraComponent.cpp
    Component.cpp
    LightComponent.cpp
    MeshComponent.cpp
    SpinComponent.cpp
    TransformComponent.cpp

  PUBLIC
    FILE_SET engineIncludes
    TYPE HEADERS
    FILES
      CameraComponent.h
      Component.h
      LightComponent.h
      MeshComponent.h
      SpinComponent.h
      TransformComponent.h
)

add_subdirectory(Internal)