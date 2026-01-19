project(Vulkan_Graphics)

set(SKAPE_VULKAN_PLUGIN_NAME "Vulkan")

SkapeAddPlugin(${SKAPE_VULKAN_PLUGIN_NAME} "" "" "")
SkapeAddGraphicsSupport("Vulkan" ${SKAPE_VULKAN_PLUGIN_NAME})
