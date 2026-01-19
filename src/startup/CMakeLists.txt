project(Startup)

add_executable(Startup)

target_sources(Startup
  PRIVATE
  main.cpp
)

target_link_libraries(Startup
  PRIVATE
    SkapeEngine
)
