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

set(AVAILABLE_PLUGINS)

# TODO: Maybe make this into an include.

set(ENGINE_INCLUDES
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/external/stb>
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/external/fastgltf/include>
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/external/fastgltf/deps>
)

macro(ListFiles)
  set(SearchRoot ${CMAKE_CURRENT_SOURCE_DIR})
  set(Args ${ARGN})
  foreach(AdditionalPath IN LISTS Args)
    set(SearchRoot "${SearchRoot}/${AdditionalPath}")
  endforeach()
  
  
  file(GLOB_RECURSE SRC_CPP "${SearchRoot}/*.cpp")
  foreach(TargetFile IN LISTS SRC_CPP)
    file(RELATIVE_PATH Relative ${CMAKE_CURRENT_SOURCE_DIR} ${TargetFile})
    message("${Relative}")
  endforeach()

  file(GLOB_RECURSE SRC_H "${SearchRoot}/*.h")
  foreach(TargetFile IN LISTS SRC_H)
    file(RELATIVE_PATH Relative ${CMAKE_CURRENT_SOURCE_DIR} ${TargetFile})
    message("${Relative}")
  endforeach()
endmacro()


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

set(SKAPE_PLUGINS_BUILD "Win64_Platform;OpenGL_Graphics;SDL3_Wrapper" CACHE STRING "The plugins to build")
set(SKAPE_PLUGINS_LINK "SDL3_Wrapper" CACHE STRING "The plugins to link. Excluding Platform and Graphics plugins.")

add_subdirectory(Plugins)

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

# Find Selected Platform Plugin
set(SKAPE_PLATFORM_PROJECT "")
if(NOT ${SKAPE_PLATFORM} STREQUAL "Invalid")
  list(FIND SKAPE_PLATFORMS "${SKAPE_PLATFORM}" PlatformIndex)
  list(GET SKAPE_PLATFORM_PLUGINS ${PlatformIndex} PlatformPlugin)
  set(SKAPE_PLATFORM_PROJECT ${PlatformPlugin})
endif()

# Find Selected Graphics Plugin
set(SKAPE_GRAPHICS_PROJECT "")
if(NOT ${SKAPE_GRAPHICS} STREQUAL "Invalid")
  list(FIND SKAPE_GRAPHICS_LIBRARIES "${SKAPE_GRAPHICS}" GraphicsApiIndex)
  list(GET SKAPE_GRAPHICS_PLUGINS ${GraphicsApiIndex} GraphicsApiPlugin)
  set(SKAPE_GRAPHICS_PROJECT ${GraphicsApiPlugin})
endif()

add_subdirectory(external)

# Create Skape Projects
add_subdirectory(src)

message("${AVAILABLE_PLUGINS}")
message("${SKAPE_PLATFORM_PROJECT}")
message("${SKAPE_GRAPHICS_PROJECT}")
