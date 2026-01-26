
target_sources(SkapeEngine
  PRIVATE
    Input.cpp

  PUBLIC
    FILE_SET engineIncludes
    TYPE HEADERS
    FILES
      Gamepad.h
      Input.h
      Keyboard.h
      Mouse.h
)