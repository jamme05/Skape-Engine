

target_sources(SkapeEngine
        PRIVATE
        PlaceholderTab.cpp
        SceneViewportTab.cpp
        ObjectListTab.cpp

        PUBLIC
        FILE_SET engineIncludes
        TYPE HEADERS
        FILES
        Tab.h
        PlaceholderTab.h
        SceneViewportTab.h
        ObjectListTab.h
)
