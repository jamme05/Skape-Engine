
target_sources(Win64_Platform
	PRIVATE
    Types.cpp

  PUBLIC
    FILE_SET vulkanGraphicsIncludes
    TYPE HEADERS
    FILES
      Types.h
)

add_subdirectory(Memory)
add_subdirectory(Platform)