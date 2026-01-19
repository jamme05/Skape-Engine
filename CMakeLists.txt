cmake_minimum_required(VERSION 4.0)

project(Skape VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SKAPE_INCLUDES)
set(SKAPE_LIBRARIES)
set(SKAPE_DEFINES)

set(SKAPE_PLATFORM_PLUGINS)
set(SKAPE_GRAPHICS_PLUGINS)

set(SKAPE_PLATFORMS)
set(SKAPE_GRAPHICS_LIBRARIES)

function(SkapeAddPlugin_Impl PluginName IncludeList AdditionalLibs Defines)

endfunction()

macro(SkapeAddPlugin PluginName IncludeList AdditionalLibs Defines)
  SkapeAddPlugin_Impl(PluginName IncludeList AdditionalLibs Defines)
endmacro()


function(SkapeAddPlatformSupport_Impl Platform Plugin)
  list(APPEND SKAPE_PLATFORMS ${Platform})
  list(APPEND SKAPE_PLATFORM_PLUGINS ${Plugin})
  set(SKAPE_PLATFORMS ${SKAPE_PLATFORMS} PARENT_SCOPE)
  set(SKAPE_PLATFORM_PLUGINS ${SKAPE_PLATFORM_PLUGINS} PARENT_SCOPE)
endfunction()

macro(SkapeAddPlatformSupport Platform Plugin)
  SkapeAddPlatformSupport_Impl( ${Platform} ${Plugin} )
  set(SKAPE_PLATFORMS ${SKAPE_PLATFORMS} PARENT_SCOPE)
  set(SKAPE_PLATFORM_PLUGINS ${SKAPE_PLATFORM_PLUGINS} PARENT_SCOPE)
endmacro()

  
function(SkapeAddGraphicsSupport_Impl GraphicsLibrary Plugin)
  list(APPEND SKAPE_GRAPHICS_LIBRARIES ${GraphicsLibrary})
  list(APPEND SKAPE_GRAPHICS_PLUGINS ${Plugin})
  set(SKAPE_GRAPHICS_LIBRARIES ${SKAPE_GRAPHICS_LIBRARIES} PARENT_SCOPE)
  set(SKAPE_GRAPHICS_PLUGINS ${SKAPE_GRAPHICS_PLUGINS} PARENT_SCOPE)
endfunction()

macro(SkapeAddGraphicsSupport GraphicsLibrary Plugin)
  SkapeAddGraphicsSupport_Impl( ${GraphicsLibrary} ${Plugin} )
  set(SKAPE_GRAPHICS_LIBRARIES ${SKAPE_GRAPHICS_LIBRARIES} PARENT_SCOPE)
  set(SKAPE_GRAPHICS_PLUGINS ${SKAPE_GRAPHICS_PLUGINS} PARENT_SCOPE)
endmacro()


add_subdirectory(src)

set(SKAPE_BUILD_PLUGINS "Win64_Platform;OpenGL_Graphics;SDL3_Wrapper" CACHE STRING "The plugins to build")

add_subdirectory(Modules)

list(LENGTH SKAPE_PLATFORM_PLUGINS PlatformPluginCount)
if(PlatformPluginCount EQUAL 0)
  # Fallback to Invalid.
  set(SKAPE_PLATFORMS "Invalid")
  unset(SKAPE_PLATFORM CACHE)
elseif(SKAPE_PLATFORM)
  # No longer a valid platform, remove current selection.
  if(NOT SKAPE_PLATFORM IN_LIST SKAPE_PLATFORMS)
    unset(SKAPE_PLATFORM CACHE)
  endif()
endif()

list(LENGTH SKAPE_GRAPHICS_PLUGINS GraphicsPluginCount)
if(GraphicsPluginCount EQUAL 0)
  # Fallback to Invalid.
  set(SKAPE_GRAPHICS_LIBRARIES "Invalid")
  unset(SKAPE_GRAPHICS CACHE)
elseif(SKAPE_GRAPHICS)
  # No longer a valid graphics api, remove current selection.
  if(NOT SKAPE_GRAPHICS IN_LIST SKAPE_GRAPHICS_LIBRARIES)
    unset(SKAPE_GRAPHICS CACHE)
  endif()
endif()

# TODO: Use the current device platform
list(GET SKAPE_PLATFORMS 0 FirstPlatform)
list(GET SKAPE_GRAPHICS_LIBRARIES 0 FirstGraphicsLibrary)

message("First Platform: ${FirstPlatform}")
message("First Graphics API: ${FirstGraphicsLibrary}")

set(SKAPE_PLATFORM ${FirstPlatform} CACHE STRING "The platform plugin to use")
set_property(CACHE SKAPE_PLATFORM PROPERTY STRINGS ${SKAPE_PLATFORMS})

set(SKAPE_GRAPHICS ${FirstGraphicsLibrary} CACHE STRING "The graphics plugin to use")
set_property(CACHE SKAPE_GRAPHICS PROPERTY STRINGS ${SKAPE_GRAPHICS_LIBRARIES})