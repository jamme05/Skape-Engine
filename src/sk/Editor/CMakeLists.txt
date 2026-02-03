
# TODO: Only add to target if it's gonna build the Editor runtime.

target_sources(SkapeEngine
    PRIVATE
        Editor.cpp

    PUBLIC
    FILE_SET engineIncludes
    TYPE HEADERS
    FILES
        Editor.h
)