project(Startup)

add_executable(Startup)

target_sources(Startup
  PRIVATE
    startup/main.cpp
)

target_link_libraries(Startup
  PRIVATE
    Framework
)

set_target_properties(Startup
  PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${SKAPE_GAME_BIN}"
)

install(FILES $<TARGET_RUNTIME_DLLS:Startup> TYPE BIN)