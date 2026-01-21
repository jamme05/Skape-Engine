project(Plugins)

foreach(PluginName IN LISTS SKAPE_PLUGINS_BUILD)
  # This prevents an error if SKAPE_BUILD_PLUGINS ends with ';'
  if(NOT PluginName)
    continue()
  endif()
  
  set(SkapePluginPath "${CMAKE_CURRENT_SOURCE_DIR}/${PluginName}")
  if(IS_READABLE "${SkapePluginPath}/CMakeLists.txt")
    add_subdirectory(${PluginName})
    # message("${SkapePluginPath} YEs")
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


# Push to root
set(AVAILABLE_PLUGINS ${AVAILABLE_PLUGINS} PARENT_SCOPE)

set(SKAPE_PLATFORMS ${SKAPE_PLATFORMS} PARENT_SCOPE)
set(SKAPE_PLATFORM_PLUGINS ${SKAPE_PLATFORM_PLUGINS} PARENT_SCOPE)
set(SKAPE_GRAPHICS_LIBRARIES ${SKAPE_GRAPHICS_LIBRARIES} PARENT_SCOPE)
set(SKAPE_GRAPHICS_PLUGINS ${SKAPE_GRAPHICS_PLUGINS} PARENT_SCOPE)
