set(CMAKE_CXX_STANDARD 20)


set( SOURCES

	main.cpp
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

add_executable(gfxworkbench
	${SOURCES}
)

target_include_directories(gfxworkbench PRIVATE 
	${SDL2_INCDIR}
	${SDL2IMAGE_INCDIR}
	${WONDERGUI_INCDIRS}
	${WGSOFTGFX_INCDIRS}
)


target_link_directories(gfxworkbench PRIVATE 
	${SDL2_LIBDIR}
	${SDL2IMAGE_LIBDIR}
	${GLEW_LIBDIR}
)


target_working_directory(gfxworkbench ${WG_ROOT})

target_link_libraries(gfxworkbench PRIVATE
	wgsoftgfx
	wgopenglgfx
	wondergui 
	wgfreetypefont
	${SDL2_LIBS}
	${SDL2IMAGE_LIBS}
	${OPENGL_LIBS}
)
