
target_sources(SkapeEngine
    PRIVATE
        SelectionManager.cpp

    PUBLIC
    FILE_SET engineIncludes
    TYPE HEADERS
    FILES
        SelectionManager.h
)