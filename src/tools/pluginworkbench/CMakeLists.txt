set(CMAKE_CXX_STANDARD 20)

source_group(Files REGULAR_EXPRESSION ".*")
source_group(WG REGULAR_EXPRESSION "wg_*|wonder*")

list(TRANSFORM WONDERGEAR_SOURCES PREPEND ../../gear/ )
list(TRANSFORM WONDERGFX_SOURCES PREPEND ../../gfx/ )
list(TRANSFORM WONDERGLYPHS_SOURCES PREPEND ../../glyphs/ )
list(TRANSFORM WONDERGUI_SOURCES PREPEND ../../gui/ )
list(TRANSFORM WGFREETYPEFONT_SOURCES PREPEND ../../glyphs/freetype/ )
list(TRANSFORM WGCLIENT_SOURCES PREPEND ../../plugin/ )
list(TRANSFORM WGLEGACYGUI_SOURCES PREPEND ../../legacygui/ )
list(TRANSFORM WGLEGACYCLIENT_SOURCES PREPEND ../../plugin/ )

#==================================================================

add_library(testplugin SHARED
	testplugin.cpp
	${WONDERGEAR_SOURCES}
	${WONDERGFX_SOURCES}
	${WONDERGLYPHS_SOURCES}
	${WONDERGUI_SOURCES}
	${WGFREETYPEFONT_SOURCES}
	${WGCLIENT_SOURCES}
)


target_include_directories(testplugin PRIVATE 
	${WGPLUGIN_INCDIRS}
	${WONDERGUI_INCDIRS}
	${WGFREETYPEFONT_INCDIRS}
)

set_target_properties(testplugin PROPERTIES
    C_VISIBILITY_PRESET       hidden
    CXX_VISIBILITY_PRESET     hidden
    VISIBILITY_INLINES_HIDDEN YES
)


if(WIN32)
    target_include_directories(testplugin PRIVATE ../../../freetype/include )
    target_link_directories(testplugin PRIVATE ../freetype/lib/x64 )
elseif(APPLE)
    target_include_directories(testplugin PRIVATE ../../../freetype/include )
    target_link_directories(testplugin PRIVATE ../../../freetype/lib)
else()
    target_include_directories(testplugin PRIVATE /usr/include/freetype2 )
endif()

if(APPLE)
    target_link_libraries(testplugin PRIVATE freetype_arm64 freetype_x86_64 bz2 z )
else()
    target_link_libraries(testplugin PRIVATE freetype )
endif()

#======================================================================

add_library(legacytestplugin SHARED
	legacytestplugin.cpp
	${WONDERGEAR_SOURCES}
	${WONDERGFX_SOURCES}
	${WONDERGLYPHS_SOURCES}
	${WGLEGACYGUI_SOURCES}
	${WGFREETYPEFONT_SOURCES}
	${WGLEGACYCLIENT_SOURCES}
)

target_include_directories(legacytestplugin PRIVATE
	${WGPLUGIN_INCDIRS}
	${WGLEGACYGUI_INCDIRS}
	${WGFREETYPEFONT_INCDIRS}
)

set_target_properties(legacytestplugin PROPERTIES
    C_VISIBILITY_PRESET       hidden
    CXX_VISIBILITY_PRESET     hidden
    VISIBILITY_INLINES_HIDDEN YES
)


if(WIN32)
    target_include_directories(legacytestplugin PRIVATE ../../../freetype/include )
    target_link_directories(legacytestplugin PRIVATE ../freetype/lib/x64 )
elseif(APPLE)
    target_include_directories(legacytestplugin PRIVATE ../../../freetype/include )
    target_link_directories(legacytestplugin PRIVATE ../../../freetype/lib)
else()
    target_include_directories(legacytestplugin PRIVATE /usr/include/freetype2 )
endif()

if(APPLE)
    target_link_libraries(legacytestplugin PRIVATE freetype_arm64 freetype_x86_64 bz2 z )
else()
    target_link_libraries(legacytestplugin PRIVATE freetype )
endif()

#========================================================================

add_executable(pluginworkbench

	pluginworkbench.cpp
	pluginworkbench.h
)

target_include_directories(pluginworkbench PRIVATE 
	${WAPP_INCDIRS}
	${WGPLUGIN_INCDIRS}
)

add_dependencies(pluginworkbench testplugin legacytestplugin)

target_working_directory(pluginworkbench ${WG_ROOT} )

target_link_libraries(pluginworkbench PRIVATE
	${WAPPLIB}
	PRIVATE wgpluginhost
)


