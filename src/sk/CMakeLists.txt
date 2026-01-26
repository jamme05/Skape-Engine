project(SkapeEngine)

add_library(SkapeEngine STATIC)

target_compile_definitions(SkapeEngine PUBLIC "SK_ROOT_DIR=\"${SKAPE_GAME_DIR}\"")

target_link_libraries(SkapeEngine
  PUBLIC
    fastgltf::fastgltf
    simdjson::simdjson
    stb_image
    ${SKAPE_PLATFORM_PROJECT}
    ${SKAPE_GRAPHICS_PROJECT}
    ${SKAPE_PLUGINS_LINK}
)

target_include_directories(SkapeEngine INTERFACE ${ENGINE_INCLUDES})

target_sources(SkapeEngine
  PUBLIC
    FILE_SET engineIncludes
    TYPE HEADERS
    FILES
      sk/Skape_Main.h
)

add_subdirectory(sk/Assets)
add_subdirectory(sk/Containers)
add_subdirectory(sk/Debugging)
add_subdirectory(sk/Graphics)
add_subdirectory(sk/Helpers)
add_subdirectory(sk/Input)
add_subdirectory(sk/Macros)
add_subdirectory(sk/Math)
add_subdirectory(sk/Memory)
add_subdirectory(sk/Misc)
add_subdirectory(sk/Platform)
add_subdirectory(sk/Reflection)
add_subdirectory(sk/Scene)
add_subdirectory(sk/Seralization)

# Copy fastgltf and simdjson dynlibs into the runtime dir.
add_custom_command(
	TARGET SkapeEngine POST_BUILD
	COMMAND "${CMAKE_COMMAND}" -E copy $<TARGET_FILE:fastgltf> "${SKAPE_GAME_BIN}"
	VERBATIM
)

add_custom_command(
	TARGET SkapeEngine POST_BUILD
	COMMAND "${CMAKE_COMMAND}" -E copy $<TARGET_FILE:simdjson> "${SKAPE_GAME_BIN}"
	VERBATIM
)
