cmake_minimum_required(VERSION 3.16)

project(WonderGUI VERSION 3.0 DESCRIPTION "WonderGUI User Interface Library")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(WG_ROOT ${PROJECT_SOURCE_DIR}/..)

include_directories( 
	baselibrary
	baselibrary/base
	baselibrary/components
	baselibrary/sizebrokers
	baselibrary/skins/anim
	baselibrary/skins/meter
	baselibrary/skins
	baselibrary/skins/misc
	baselibrary/skins/state
	baselibrary/skins/static
	baselibrary/textlayouts
	baselibrary/numberlayouts
	baselibrary/widgets
	baselibrary/widgets/capsules
	baselibrary/widgets/layers
	baselibrary/widgets/lists
	baselibrary/widgets/panels

	gfxdevices/software
	gfxdevices/opengl
	gfxdevices/stream
	gfxdevices/metal

	fontengines/freetype

	../SDL2/include
	../SDL2_image/include
	../glew/include
)

link_directories(
	../SDL2/lib/x64
	../SDL2_image/lib/x64
	../freetype/lib/x64
	../glew/lib/Release/x64
    ../freetype/lib
)

if(WIN32)

	set(SDL2_LIBS SDL2 SDL2main)
	set(SDL2IMAGE_LIBS SDL2_image)

	set(OPENGL_LIBS glew32 opengl32)

elseif(APPLE)

	find_library(SDL2_LIBS SDL2)
	find_library(SDL2IMAGE_LIBS SDL2_image)

	find_library(OPENGL_LIBS OpenGL)

else()

	set(SDL2_LIBS SDL2)
	set(SDL2IMAGE_LIBS SDL2_image)

	set(OPENGL_LIBS GLEW GL)
endif()


function( target_working_directory target workpath )

	if(WIN32)
		set_target_properties( ${target} PROPERTIES 
			VS_DEBUGGER_WORKING_DIRECTORY ${workpath} 
		)

#		set_property(TARGET ${target} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${workpath} )

	elseif(APPLE)
		set_target_properties( ${target} PROPERTIES
			XCODE_GENERATE_SCHEME TRUE
			XCODE_SCHEME_WORKING_DIRECTORY "${workpath}"
		)
	endif()

endfunction()


add_subdirectory(baselibrary)
add_subdirectory(gfxdevices/software)
add_subdirectory(gfxdevices/opengl)
if(APPLE)
	add_subdirectory(gfxdevices/metal)
endif()
add_subdirectory(fontengines/freetype)
add_subdirectory(examples)
add_subdirectory(tools/gfxworkbench)
add_subdirectory(tools/workbench)
add_subdirectory(tools/gfxdevicetester)
add_subdirectory(tools/streamgenerator)
add_subdirectory(tools/streamanalyzer)
#add_subdirectory(tools/unittester)
add_subdirectory(tools/spxworkbench)

if(MSVC)
	target_compile_options(gfxdevicetester PRIVATE "/MP")
	target_compile_options(gfxworkbench PRIVATE "/MP")
	target_compile_options(spxworkbench PRIVATE "/MP")
	target_compile_options(wgsoftgfx PRIVATE "/MP")
	target_compile_options(wgopenglgfx PRIVATE "/MP")
	target_compile_options(wondergui PRIVATE "/MP")
	target_compile_options(workbench PRIVATE "/MP")
endif()
