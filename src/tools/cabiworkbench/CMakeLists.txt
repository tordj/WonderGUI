set(CMAKE_CXX_STANDARD 20)

source_group(Files REGULAR_EXPRESSION ".*")
source_group(WG REGULAR_EXPRESSION "wg_*|wonder*")

list(TRANSFORM WONDERGEAR_SOURCES PREPEND ../../gear/ )
list(TRANSFORM WONDERGFX_SOURCES PREPEND ../../gfx/ )
list(TRANSFORM WONDERGLYPHS_SOURCES PREPEND ../../glyphs/ )
list(TRANSFORM WONDERGUI_SOURCES PREPEND ../../gui/ )
list(TRANSFORM WGFREETYPEFONT_SOURCES PREPEND ../../glyphs/freetype/ )


add_library(cabiclient SHARED
	cabiclient.cpp
	${WONDERGEAR_SOURCES}
	${WONDERGFX_SOURCES}
	${WONDERGLYPHS_SOURCES}
	${WONDERGUI_SOURCES}
	${WGFREETYPEFONT_SOURCES}
	
	../../cabi/wg_cabi.h
	../../cabi/wg_cabi_base.cpp
	../../cabi/wg_cabi_base.h
	../../cabi/wg_cabi_bitmapcache.cpp
	../../cabi/wg_cabi_bitmapcache.h
	../../cabi/wg_cabi_bitmapfont.cpp
	../../cabi/wg_cabi_bitmapfont.h
	../../cabi/wg_cabi_canvaslayers.cpp
	../../cabi/wg_cabi_canvaslayers.h
	../../cabi/wg_cabi_font.cpp
	../../cabi/wg_cabi_font.h
	../../cabi/wg_cabi_freetypefont.cpp
	../../cabi/wg_cabi_freetypefont.h
	../../cabi/wg_cabi_gfxdevice.cpp
	../../cabi/wg_cabi_gfxdevice.h
	../../cabi/wg_cabi_hostbridge.cpp
	../../cabi/wg_cabi_hostbridge.h
	../../cabi/wg_cabi_root.cpp
	../../cabi/wg_cabi_root.h
	../../cabi/wg_cabi_root_incalls.cpp
	../../cabi/wg_cabi_root_incalls.h
	../../cabi/wg_cabi_root_interface.h
	../../cabi/wg_cabi_surface.cpp
	../../cabi/wg_cabi_surface.h
	../../cabi/wg_cabi_surfacefactory.cpp
	../../cabi/wg_cabi_surfacefactory.h
)


target_include_directories(cabiclient PRIVATE 
	${WGCABI_INCDIRS}
	${WONDERGUI_INCDIRS}
	${WGFREETYPEFONT_INCDIRS}
	${WGFREETYPEFONT_INCDIRS}
	${WGCABI_INCDIRS}
)

set_target_properties(cabiclient PROPERTIES
    C_VISIBILITY_PRESET       hidden
    CXX_VISIBILITY_PRESET     hidden
    VISIBILITY_INLINES_HIDDEN YES
)


if(WIN32)
    target_include_directories(cabiclient PRIVATE ../../../freetype/include )
    target_link_directories(cabiclient PRIVATE ../freetype/lib/x64 )
elseif(APPLE)
    target_include_directories(cabiclient PRIVATE ../../../freetype/include )
    target_link_directories(cabiclient PRIVATE ../../../freetype/lib)
else()
    target_include_directories(cabiclient PRIVATE /usr/include/freetype2 )
endif()

if(APPLE)
    target_link_libraries(cabiclient PRIVATE freetype_arm64 freetype_x86_64 bz2 z )
else()
    target_link_libraries(cabiclient PRIVATE freetype )
endif()


add_executable(cabiworkbench

	cabiworkbench.cpp
	cabiworkbench.h
)

target_include_directories(cabiworkbench PRIVATE 
	${WAPP_INCDIRS}
	${WGCABI_INCDIRS}
)

add_dependencies(cabiworkbench cabiclient)

target_working_directory(cabiworkbench ${WG_ROOT} )

target_link_libraries(cabiworkbench PRIVATE
	${WAPPLIB}
	PRIVATE wgcabihost
)


