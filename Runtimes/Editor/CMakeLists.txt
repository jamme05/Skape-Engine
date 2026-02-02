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
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/editor"
)

install(FILES $<TARGET_RUNTIME_DLLS:Startup> TYPE BIN)