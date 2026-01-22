SkapePlugin(Vulkan_Graphics)

SkapeAddGraphicsSupport(Vulkan Vulkan_Graphics)

target_compile_definitions(Vulkan_Graphics PUBLIC SK_GRAPHICS_VULKAN)


target_sources(Vulkan_Graphics
	PRIVATE
    sk/Assets/Mesh.cpp
    sk/Graphics/Renderer_Impl.cpp
    sk/Graphics/Rendering/Render_Context.cpp
    sk/Graphics/Rendering/Window_Context.cpp
    sk/Types.cpp

  PUBLIC
    FILE_SET vulkanGraphicsIncludes
    TYPE HEADERS
    FILES
      sk/Assets/Mesh.h
      sk/Graphics/Renderer_Impl.h
      sk/Graphics/Rendering/Render_Context.h
      sk/Graphics/Rendering/Window_Context.h
      sk/Types.h
)
