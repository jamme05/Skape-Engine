target_sources(OpenGL_Graphics
	PRIVATE
    Renderer_Impl.cpp

  PUBLIC
    FILE_SET openGlGraphicsIncludes
    TYPE HEADERS
    FILES
      Renderer_Impl.h
)

add_subdirectory(Buffer)
add_subdirectory(Rendering)
add_subdirectory(Utils)

