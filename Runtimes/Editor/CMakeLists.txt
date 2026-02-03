SkapeRuntime(Skape_Editor)

add_executable(Skape_Editor)

list(APPEND SKAPE_DEFINES "SKAPE_EDITOR_AVAILABLE")
set(SKAPE_DEFINES ${SKAPE_DEFINES} PARENT_SCOPE)
list(APPEND SKAPE_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}")
set(SKAPE_INCLUDES ${SKAPE_INCLUDES} PARENT_SCOPE)

target_sources(Skape_Editor
    PUBLIC
    FILE_SET skapeEditorIncludes
    TYPE HEADERS
    BASE_DIRS
        ./
)

target_link_libraries(Skape_Editor
  PRIVATE
    Framework
)

set_target_properties(Skape_Editor
  PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${SKAPE_GAME_BIN}"
)
add_subdirectory(sk)
