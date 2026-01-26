target_sources(OpenGL_Graphics
	PRIVATE
    Depth_Target.cpp
    Frame_Buffer.cpp
    Render_Context.cpp
    Render_Target.cpp
    Window_Context.cpp

  PUBLIC
    FILE_SET openGlGraphicsIncludes
    TYPE HEADERS
    FILES
      Depth_Target.h
      Frame_Buffer.h
      Render_Context.h
      Render_Target.h
      Scissor.h
      Viewport.h
      Window_Context.h
)