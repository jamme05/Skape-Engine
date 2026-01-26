SkapePlugin(Win64_Platform)

SkapeAddPlatformSupport(Win64 Win64_Platform)

target_compile_definitions(Win64_Platform PUBLIC SK_PLATFORM_WINDOWS)

target_sources(Win64_Platform
	PUBLIC
    FILE_SET openGlGraphicsIncludes
		TYPE HEADERS
    BASE_DIRS
      ./
)

add_subdirectory(sk)