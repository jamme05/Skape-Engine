
target_sources(SkapeEngine
  PRIVATE
    Forward_Pass.cpp
    GBuffer_Pass.cpp
    Light_Pass.cpp
    Render_Pass.cpp
    Screen_Pass.cpp
    
  PUBLIC
    FILE_SET engineIncludes
    TYPE HEADERS
    FILES
      Forward_Pass.h
      GBuffer_Pass.h
      Light_Pass.h
      Render_Pass.h
      Screen_Pass.h
)