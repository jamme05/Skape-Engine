

target_sources(SkapeEngine
        PRIVATE
        AssetViewTab.cpp
        PlaceholderTab.cpp
        SceneViewportTab.cpp
        ObjectListTab.cpp

        PUBLIC
        FILE_SET engineIncludes
        TYPE HEADERS
        FILES
        Tab.h
        AssetViewTab.h
        PlaceholderTab.h
        SceneViewportTab.h
        ObjectListTab.h
)
