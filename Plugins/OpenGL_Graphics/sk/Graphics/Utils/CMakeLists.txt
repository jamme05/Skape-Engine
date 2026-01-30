target_sources(OpenGL_Graphics
	PRIVATE
    Shader_Link.cpp
    Shader_Reflection.cpp
    RenderUtils.cpp

  PUBLIC
    FILE_SET openGlGraphicsIncludes
    TYPE HEADERS
    FILES
      Shader_Link.h
      Shader_Reflection.h
		RenderUtils_Ex.h
)