# Plugin Variables

set(AVAILABLE_RUNTIMES)

set(SKAPE_RUNTIMES_BUILD "Editor" CACHE STRING "The Runtimes to build")

# Runtime Setup Macros

# TODO: Add a way to add dependencies to a runtime
macro(SkapeRuntime RuntimeName)
  project(${RuntimeName})

  list(APPEND AVAILABLE_RUNTIMES ${RuntimeName})
  set(AVAILABLE_RUNTIMES ${AVAILABLE_RUNTIMES} PARENT_SCOPE)
endmacro()

# Runtime Setup

foreach(RuntimeName IN LISTS SKAPE_RUNTIMES_BUILD)
  # This prevents an error if SKAPE_RUNTIMES_BUILD ends with ';'
  if(NOT RuntimeName)
    continue()
  endif()
  
  set(SkapePluginPath "${CMAKE_CURRENT_SOURCE_DIR}/Runtimes/${RuntimeName}")
  if(IS_READABLE "${SkapePluginPath}/CMakeLists.txt")
    add_subdirectory("Runtimes/${RuntimeName}")
  else()
    message("Error: Runtime with name \"${RuntimeName}\" not found!")
  endif()
endforeach()
