
set(CMAKE_CXX_STANDARD 20)

set( SOURCES

	main.cpp
	sdl_wglib.cpp
	sdl_wglib.h
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

add_executable(legacyworkbench
	${SOURCES}
)


target_include_directories(legacyworkbench PRIVATE
	${SDL2_INCDIR}
	${SDL2IMAGE_INCDIR}
	${WGLEGACYGUI_INCDIRS}
	${GFXSTREAM_INCDIRS}
	${WGSOFTGFX_INCDIRS}
	${WGOPENGLGFX_INCDIRS}
)

target_link_directories(legacyworkbench PRIVATE 
	${FREETYPE2_LIBDIR}
	${SDL2_LIBDIR}
	${SDL2IMAGE_LIBDIR}
	${GLEW_LIBDIR}
)

target_working_directory(legacyworkbench ${WG_ROOT} )


target_link_libraries(legacyworkbench PRIVATE
	wgsoftgfx
	wgopenglgfx
	wggfxstream
	wglegacygui
	${SDL2_LIBS}
	${SDL2IMAGE_LIBS}
	${OPENGL_LIBS}
	${FREETYPE2_LIB}
)

