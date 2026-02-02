# Plugin Variables

set(SKAPE_PLATFORM_PLUGINS)
set(SKAPE_GRAPHICS_PLUGINS)

set(SKAPE_PLATFORMS)
set(SKAPE_GRAPHICS_LIBRARIES)

set(AVAILABLE_PLUGINS)

set(SKAPE_PLUGINS_BUILD "Win64_Platform;OpenGL_Graphics;SDL3_Wrapper" CACHE STRING "The plugins to build")
set(SKAPE_PLUGINS_LINK "SDL3_Wrapper" CACHE STRING "The plugins to link. Excluding Platform and Graphics plugins.")


# Plugin Setup Macros
# TODO: Check if its possible to lookup by project name rather than by path
macro(SkapePlugin PluginName)
  project(${PluginName})
  add_library(${PluginName} STATIC)
  target_include_directories(${PluginName} PUBLIC ${ENGINE_INCLUDES})

  list(APPEND AVAILABLE_PLUGINS ${PluginName})
  set(AVAILABLE_PLUGINS ${AVAILABLE_PLUGINS} PARENT_SCOPE)
endmacro()

macro(SkapeAddPlatformSupport Platform Plugin)
  list(APPEND SKAPE_PLATFORMS ${Platform})
  list(APPEND SKAPE_PLATFORM_PLUGINS ${Plugin})
  set(SKAPE_PLATFORMS ${SKAPE_PLATFORMS} PARENT_SCOPE)
  set(SKAPE_PLATFORM_PLUGINS ${SKAPE_PLATFORM_PLUGINS} PARENT_SCOPE)
endmacro()

macro(SkapeAddGraphicsSupport GraphicsLibrary Plugin)
  list(APPEND SKAPE_GRAPHICS_LIBRARIES ${GraphicsLibrary})
  list(APPEND SKAPE_GRAPHICS_PLUGINS ${Plugin})
  set(SKAPE_GRAPHICS_LIBRARIES ${SKAPE_GRAPHICS_LIBRARIES} PARENT_SCOPE)
  set(SKAPE_GRAPHICS_PLUGINS ${SKAPE_GRAPHICS_PLUGINS} PARENT_SCOPE)
endmacro()


# Plugin Setup

foreach(PluginName IN LISTS SKAPE_PLUGINS_BUILD)
  # This prevents an error if SKAPE_BUILD_PLUGINS ends with ';'
  if(NOT PluginName)
    continue()
  endif()
  
  set(SkapePluginPath "${CMAKE_CURRENT_SOURCE_DIR}/Plugins/${PluginName}")
  if(IS_READABLE "${SkapePluginPath}/CMakeLists.txt")
    add_subdirectory("Plugins/${PluginName}")
  else()
    message("Error: Plugin with name \"${PluginName}\" not found!")
  endif()
endforeach()

foreach(PluginName IN LISTS SKAPE_PLUGINS_LINK)
  if(NOT PluginName)
    continue()
  endif()
  if(NOT ${PluginName} IN_LIST AVAILABLE_PLUGINS)
    message("Error: Can't link plugin with name \"${PluginName}\" as it won't be built.")
  endif()
  if(${PluginName} IN_LIST SKAPE_PLATFORM_PLUGINS)
    message("Error: Can't link plugin with name \"${PluginName}\" as it won't be built.")
  endif()
  if(${PluginName} IN_LIST SKAPE_GRAPHICS_PLUGINS)
    message("Error: Can't link plugin with name \"${PluginName}\" as it won't be built.")
  endif()
endforeach()
