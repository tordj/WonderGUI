set(SOURCES
    wg_debugoverlay.h
    wg_debugoverlay.cpp
)

set(INCDIRS
   ${CMAKE_CURRENT_LIST_DIR}
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

add_library( wgdebugger STATIC ${SOURCES} )

target_include_directories(wgdebugger PRIVATE
    ${INCDIRS}
    ${WONDERGUI_INCDIRS}
)

set_target_properties( wgdebugger PROPERTIES FOLDER "Extensions" )


list(TRANSFORM SOURCES PREPEND ${CMAKE_CURRENT_LIST_DIR}/ )
set(WGDEBUGGER_SOURCES ${SOURCES} PARENT_SCOPE)
set(WGDEBUGGER_INCDIRS ${INCDIRS} PARENT_SCOPE)