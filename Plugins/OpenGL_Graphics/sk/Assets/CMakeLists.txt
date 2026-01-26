target_sources(OpenGL_Graphics
	PRIVATE
    Shader.cpp
    Texture.cpp

  PUBLIC
    FILE_SET openGlGraphicsIncludes
    TYPE HEADERS
    FILES
      Shader.h
      Texture.h
)