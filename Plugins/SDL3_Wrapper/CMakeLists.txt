SkapePlugin(SDL3_Wrapper)

set(SKAPE_SDL3_PLUGIN_NAME "SDL3")

target_compile_definitions(SDL3_Wrapper PUBLIC SK_CUSTOM_MAIN)

add_subdirectory(SDL EXCLUDE_FROM_ALL)

target_sources(SDL3_Wrapper
	PRIVATE
		sk/Types.cpp
		sk/Window/Window.cpp
		sk/Window/Window_OpenGL.cpp
		sk/Window/Window_VK.cpp

	PUBLIC
    FILE_SET sdl3WrapperIncludes
    TYPE HEADERS
    FILES
			sk/Input/Gamepad_Include.h
			sk/Input/Keyboard_Include.h
			sk/Input/Mouse_Include.h
			sk/Types.h
			sk/Window/Window.h
)

target_link_libraries(SDL3_Wrapper
	PUBLIC
		SDL3::SDL3
)

add_custom_command(
	TARGET SDL3_Wrapper POST_BUILD
	COMMAND "${CMAKE_COMMAND}" -E copy $<TARGET_FILE:SDL3::SDL3-shared> "${SKAPE_GAME_BIN}"
	VERBATIM
)
