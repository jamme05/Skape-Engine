SkapeRuntime(Skape_Editor)

add_executable(Skape_Editor)

target_sources(Skape_Editor
  PRIVATE
    sk/main.cpp
)

target_link_libraries(Skape_Editor
  PRIVATE
    Framework
)

set_target_properties(Skape_Editor
  PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${SKAPE_GAME_BIN}"
)
