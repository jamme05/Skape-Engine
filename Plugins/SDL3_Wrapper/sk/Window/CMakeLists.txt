target_sources(SDL3_Wrapper
	PRIVATE
		Window.cpp
		Window_OpenGL.cpp
		Window_VK.cpp

	PUBLIC
    FILE_SET sdl3WrapperIncludes
    TYPE HEADERS
    FILES
			Window.h
)
