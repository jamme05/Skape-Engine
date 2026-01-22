SkapePlugin(Win64_Platform)

SkapeAddPlatformSupport(Win64 Win64_Platform)

target_sources(Win64_Platform
	PRIVATE
    sk/Memory/Tracker.cpp
    sk/Platform/Platform.cpp
    sk/Platform/Time.cpp
    sk/Types.cpp

  PUBLIC
    FILE_SET vulkanGraphicsIncludes
    TYPE HEADERS
    FILES
      sk/Types.h
)
