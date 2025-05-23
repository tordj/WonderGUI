set(SOURCES
    wg_simplistic.h
    wg_simplistic.cpp
)


add_library(wg_theme_simplistic STATIC ${SOURCES} )

target_include_directories(wg_theme_simplistic PRIVATE 
    .
    ${WONDERGUI_INCDIRS}
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

set_target_properties( wg_theme_simplistic PROPERTIES FOLDER "Themes" )
