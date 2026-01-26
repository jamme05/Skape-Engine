
target_sources(SkapeEngine
  PRIVATE
    CameraManager.cpp
    EventManager.cpp
    Internal_Component_Manager.cpp
    Layer_Manager.cpp
    Light_Manager.cpp
    SceneManager.cpp

  PUBLIC
    FILE_SET engineIncludes
    TYPE HEADERS
    FILES
      CameraManager.h
      EventManager.h
      Internal_Component_Manager.h
      Layer_Manager.h
      Light_Manager.h
      SceneManager.h
)