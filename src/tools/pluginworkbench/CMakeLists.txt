set(CMAKE_CXX_STANDARD 20)

source_group(Files REGULAR_EXPRESSION ".*")
source_group(WG REGULAR_EXPRESSION "wg_*|wonder*")

#==================================================================

add_library(testplugin SHARED
	testplugin.cpp
	${WONDERGEAR_SOURCES}
	${WONDERGFX_SOURCES}
	${WONDERGUI_SOURCES}
	${WGFREETYPEFONT_SOURCES}
	${WGPLUGIN_SOURCES}
)


target_include_directories(testplugin PRIVATE 
	${WGPLUGIN_INCDIRS}
	${WONDERGUI_INCDIRS}
	${WGFREETYPEFONT_INCDIRS}
	${SDL2_INCDIR}
)

set_target_properties(testplugin PROPERTIES
    C_VISIBILITY_PRESET       hidden
    CXX_VISIBILITY_PRESET     hidden
    VISIBILITY_INLINES_HIDDEN YES
)

target_include_directories(testplugin PRIVATE ${FREETYPE2_INCDIR} )
target_link_directories(testplugin PRIVATE ${FREETYPE2_LIBDIR} )
target_link_libraries(testplugin PRIVATE ${FREETYPE2_LIB} )

#======================================================================

add_library(legacytestplugin SHARED
	legacytestplugin.cpp
	${WONDERGEAR_SOURCES}
	${WONDERGFX_SOURCES}
	${WONDERGUI_SOURCES}
	${WGLEGACYGUI_SOURCES}
	${WGFREETYPEFONT_SOURCES}
	${WGLEGACYPLUGIN_SOURCES}
)

target_include_directories(legacytestplugin PRIVATE
	${WGPLUGIN_INCDIRS}
	${WGLEGACYGUI_INCDIRS}
	${WGFREETYPEFONT_INCDIR}
)

set_target_properties(legacytestplugin PROPERTIES
    C_VISIBILITY_PRESET       hidden
    CXX_VISIBILITY_PRESET     hidden
    VISIBILITY_INLINES_HIDDEN YES
)

target_include_directories(legacytestplugin PRIVATE ${FREETYPE2_INCDIR} )
target_link_directories(legacytestplugin PRIVATE ${FREETYPE2_LIBDIR} )
target_link_libraries(legacytestplugin PRIVATE ${FREETYPE2_LIB} wggfxstream wgsoftgfx )

#========================================================================

add_executable(pluginworkbench

	pluginworkbench.cpp
	pluginworkbench.h
)

target_include_directories(pluginworkbench PRIVATE 
	${WAPP_INCDIRS}
	${WGPLUGIN_INCDIRS}
	${SDL2_INCDIR}
)

add_dependencies(pluginworkbench testplugin legacytestplugin)

target_working_directory(pluginworkbench ${WG_ROOT} )

target_link_libraries(pluginworkbench PRIVATE
	${WAPPLIB}
	PRIVATE wgpluginhost
)


