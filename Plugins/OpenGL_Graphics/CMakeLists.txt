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
	PRIVATE
    sk/Assets/Shader.cpp
    sk/Assets/Texture.cpp
    sk/Graphics/Buffer/Unsafe_Buffer.cpp
    sk/Graphics/Renderer_Impl.cpp
    sk/Graphics/Rendering/Depth_Target.cpp
    sk/Graphics/Rendering/Frame_Buffer.cpp
    sk/Graphics/Rendering/Render_Context.cpp
    sk/Graphics/Rendering/Render_Target.cpp
    sk/Graphics/Rendering/Window_Context.cpp
    sk/Graphics/Utils/Shader_Link.cpp
    sk/Graphics/Utils/Shader_Reflection.cpp
    sk/Types.cpp

  PUBLIC
    FILE_SET openGlGraphicsIncludes
    TYPE HEADERS
    FILES
      sk/Assets/Shader.h
      sk/Assets/Texture.h
      sk/Graphics/Buffer/Unsafe_Buffer.h
      sk/Graphics/Renderer_Impl.h
      sk/Graphics/Rendering/Depth_Target.h
      sk/Graphics/Rendering/Frame_Buffer.h
      sk/Graphics/Rendering/Render_Context.h
      sk/Graphics/Rendering/Render_Target.h
      sk/Graphics/Rendering/Scissor.h
      sk/Graphics/Rendering/Viewport.h
      sk/Graphics/Rendering/Window_Context.h
      sk/Graphics/Utils/Shader_Link.h
      sk/Graphics/Utils/Shader_Reflection.h
      sk/Types.h
)

add_custom_command(
	TARGET OpenGL_Graphics POST_BUILD
	COMMAND "${CMAKE_COMMAND}" -E copy $<TARGET_FILE:glbinding> "${SKAPE_GAME_BIN}"
	VERBATIM
)
