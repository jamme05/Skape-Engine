
target_sources(SkapeEngine
  PRIVATE
    Asset.cpp
    Material.cpp
    Mesh.cpp
    Model.cpp

  PUBLIC
    FILE_SET engineIncludes
    TYPE HEADERS
    FILES
      Asset.h
      Material.h
      Mesh.h
      Model.h
)

add_subdirectory(Access)
add_subdirectory(Management)
add_subdirectory(Utils)
add_subdirectory(Workers)
