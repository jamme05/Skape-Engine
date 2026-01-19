project(Modules)

set(TargetPlugins )

foreach(ModuleName IN LISTS SKAPE_BUILD_PLUGINS)
  # This prevents an error if SKAPE_BUILD_PLUGINS ends with ';'
  if(NOT ModuleName)
    continue()
  endif()
  
  set(SkapeModulePath "${CMAKE_CURRENT_SOURCE_DIR}/${ModuleName}")
  if(IS_READABLE "${SkapeModulePath}/CMakeLists.txt")
    add_subdirectory(${ModuleName})
    # message("${SkapeModulePath} YEs")
  else()
    message("Error: Module with name \"${ModuleName}\" not found!")
  endif()
endforeach()

# Push to root
set(SKAPE_PLATFORMS ${SKAPE_PLATFORMS} PARENT_SCOPE)
set(SKAPE_PLATFORM_PLUGINS ${SKAPE_PLATFORM_PLUGINS} PARENT_SCOPE)
set(SKAPE_GRAPHICS_LIBRARIES ${SKAPE_GRAPHICS_LIBRARIES} PARENT_SCOPE)
set(SKAPE_GRAPHICS_PLUGINS ${SKAPE_GRAPHICS_PLUGINS} PARENT_SCOPE)
