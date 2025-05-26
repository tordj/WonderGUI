set(SOURCES
    wg_steel_n_red.h
    wg_steel_n_red.cpp
)


add_library(wg_theme_steel_n_red STATIC ${SOURCES} )

target_include_directories(wg_theme_steel_n_red PRIVATE 
    .
    ${WONDERGUI_INCDIRS}
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

set_target_properties( wg_theme_steel_n_red PROPERTIES FOLDER "Themes" )
