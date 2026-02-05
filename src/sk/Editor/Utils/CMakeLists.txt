
target_sources(SkapeEngine
    PRIVATE
        ContextMenu.cpp

    PUBLIC
    FILE_SET engineIncludes
    TYPE HEADERS
    FILES
        ContextMenu.h
)
