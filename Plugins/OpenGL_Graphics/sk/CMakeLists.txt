target_sources(OpenGL_Graphics
	PRIVATE
    Types.cpp

  PUBLIC
    FILE_SET openGlGraphicsIncludes
    TYPE HEADERS
    FILES
      Types.h
)

add_subdirectory(Assets)
add_subdirectory(Graphics)