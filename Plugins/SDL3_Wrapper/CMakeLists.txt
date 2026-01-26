SkapePlugin(SDL3_Wrapper)

set(SKAPE_SDL3_PLUGIN_NAME "SDL3")

target_compile_definitions(SDL3_Wrapper PUBLIC SK_CUSTOM_MAIN SK_PLATFORM_SUPPORTS_MOUSE SK_PLATFORM_SUPPORTS_KEYBOARD SK_PLATFORM_SUPPORTS_GAMEPAD)

add_subdirectory(SDL EXCLUDE_FROM_ALL)

target_sources(SDL3_Wrapper
	PUBLIC
    FILE_SET openGlGraphicsIncludes
		TYPE HEADERS
    BASE_DIRS
      ./
)

add_subdirectory(sk)

target_link_libraries(SDL3_Wrapper
	PUBLIC
		SDL3::SDL3
)

add_custom_command(
	TARGET SDL3_Wrapper POST_BUILD
	COMMAND "${CMAKE_COMMAND}" -E copy $<TARGET_FILE:SDL3::SDL3-shared> "${SKAPE_GAME_BIN}"
	VERBATIM
)
