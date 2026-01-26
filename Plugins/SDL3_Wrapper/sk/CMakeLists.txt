target_sources(SDL3_Wrapper
	PRIVATE
		Types.cpp

	PUBLIC
    FILE_SET sdl3WrapperIncludes
    TYPE HEADERS
    FILES
			Types.h
)

add_subdirectory(Input)
add_subdirectory(Window)