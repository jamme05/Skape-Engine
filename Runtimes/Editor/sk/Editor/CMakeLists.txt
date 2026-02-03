
target_sources(Skape_Editor
    PRIVATE
        Editor.cpp

    PUBLIC
    FILE_SET skapeEditorIncludes
    TYPE HEADERS
    FILES
        Editor.h
)