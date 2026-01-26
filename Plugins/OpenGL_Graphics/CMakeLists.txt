SkapePlugin(OpenGL_Graphics)

SkapeAddGraphicsSupport(OpenGL OpenGL_Graphics)

target_compile_definitions(OpenGL_Graphics PUBLIC
  SK_GRAPHICS_OPENGL
  SK_OPENGL_MAJOR_VERSION=4
  SK_OPENGL_MINOR_VERSION=5
)

add_subdirectory(glbinding)

target_link_libraries(OpenGL_Graphics
  PUBLIC
    glbinding::glbinding
)

target_sources(OpenGL_Graphics
	PUBLIC
    FILE_SET openGlGraphicsIncludes
		TYPE HEADERS
    BASE_DIRS
      sk/
)

add_subdirectory(sk)

add_custom_command(
	TARGET OpenGL_Graphics POST_BUILD
	COMMAND "${CMAKE_COMMAND}" -E copy $<TARGET_FILE:glbinding> "${SKAPE_GAME_BIN}"
	VERBATIM
)
