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
