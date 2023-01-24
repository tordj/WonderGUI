cmake_minimum_required(VERSION 3.16)

set_property(GLOBAL PROPERTY USE_FOLDERS YES)

project(WonderGUI VERSION 3.0 DESCRIPTION "WonderGUI User Interface Library")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(WG_ROOT ${PROJECT_SOURCE_DIR}/..)



set(WAPPLIB wappsdl2sw)

function( target_working_directory target workpath )

	if(WIN32)
		set_target_properties( ${target} PROPERTIES 
			VS_DEBUGGER_WORKING_DIRECTORY ${workpath} 
		)

	elseif(APPLE)
		set_target_properties( ${target} PROPERTIES
			XCODE_GENERATE_SCHEME TRUE
			XCODE_SCHEME_WORKING_DIRECTORY "${workpath}"
		)
	endif()

endfunction()


if(MSVC)
	add_compile_options("/MP")
endif()


set(WONDERGEAR_INCDIRS
	${PROJECT_SOURCE_DIR}/gear
	${PROJECT_SOURCE_DIR}/gear/c-api
	)

set(WONDERGFX_INCDIRS
	${WONDERGEAR_INCDIRS}
	${PROJECT_SOURCE_DIR}/gfx
	${PROJECT_SOURCE_DIR}/gfx/c-api
	${PROJECT_SOURCE_DIR}/gfx/stream
	${PROJECT_SOURCE_DIR}/gfx/stream/c-api
)

set(WONDERGLYPHS_INCDIRS
	${WONDERGFX_INCDIRS}
	${PROJECT_SOURCE_DIR}/glyphs
	${PROJECT_SOURCE_DIR}/glyphs/c-api
)

set(WONDERGUI_INCDIRS
	${WONDERGLYPHS_INCDIRS}
	${PROJECT_SOURCE_DIR}/gui
	${PROJECT_SOURCE_DIR}/gui/base/
	${PROJECT_SOURCE_DIR}/gui/c-api/
	${PROJECT_SOURCE_DIR}/gui/components/
	${PROJECT_SOURCE_DIR}/gui/numberlayouts/
	${PROJECT_SOURCE_DIR}/gui/skins/
	${PROJECT_SOURCE_DIR}/gui/skins/anim/
	${PROJECT_SOURCE_DIR}/gui/skins/meter/
	${PROJECT_SOURCE_DIR}/gui/skins/misc/
	${PROJECT_SOURCE_DIR}/gui/skins/state/
	${PROJECT_SOURCE_DIR}/gui/skins/static/
	${PROJECT_SOURCE_DIR}/gui/slots/
	${PROJECT_SOURCE_DIR}/gui/textlayouts/
	${PROJECT_SOURCE_DIR}/gui/widgets/
	${PROJECT_SOURCE_DIR}/gui/widgets/capsules/
	${PROJECT_SOURCE_DIR}/gui/widgets/lists/
	${PROJECT_SOURCE_DIR}/gui/widgets/overlays/
	${PROJECT_SOURCE_DIR}/gui/widgets/panels/
)

set(WGSOFTGFX_INCDIRS
	${PROJECT_SOURCE_DIR}/gfx/software/
	${PROJECT_SOURCE_DIR}/gfx/software/c-api
)

set(WGOPENGLGFX_INCDIRS
	${PROJECT_SOURCE_DIR}/gfx/opengl/
)

set(WGMETALGFX_INCDIRS
	${PROJECT_SOURCE_DIR}/gfx/metal/
)

set(WGFREETYPEFONT_INCDIRS
	${PROJECT_SOURCE_DIR}/glyphs/freetype/
)

set(WGCABI_INCDIRS
	${PROJECT_SOURCE_DIR}/cabi
	${PROJECT_SOURCE_DIR}/cabi/c-api
)



set(WAPP_INCDIRS
	${WONDERGUI_INCDIRS}
	${WGFREETYPEFONT_INCDIRS}

	${PROJECT_SOURCE_DIR}/wonderapp/include
	${PROJECT_SOURCE_DIR}
)

link_directories(

    ../freetype/lib

	../freetype/lib/x64
	../SDL2/lib/x64
	../SDL2_image/lib/x64
	../glew/lib/Release/x64
)


add_subdirectory(gear)
add_subdirectory(gfx)
add_subdirectory(glyphs)
add_subdirectory(gui)

add_subdirectory(glyphs/freetype)

add_subdirectory(gfx/metal)
add_subdirectory(gfx/opengl)
add_subdirectory(gfx/software)


add_subdirectory(cabi)
add_subdirectory(wonderapp/${WAPPLIB})
add_subdirectory(tools)
add_subdirectory(examples)

