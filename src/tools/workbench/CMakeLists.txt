
set(CMAKE_CXX_STANDARD 20)

source_group(Files REGULAR_EXPRESSION ".*")

add_executable(workbench

	main.cpp
)


target_working_directory(workbench ${WG_ROOT} )

target_include_directories(workbench PRIVATE 
	${WONDERGUI_INCDIRS}
	${WGSOFTGFX_INCDIRS}
	${WGOPENGLGFX_INCDIRS}
	${WGFREETYPEFONT_INCDIRS}
	${SDL2_INCDIR}
	${SDL2IMAGE_INCDIR}
)

target_link_directories(workbench PRIVATE 
	${SDL2_LIBDIR}
	${SDL2IMAGE_LIBDIR}
	${GLEW_LIBDIR}
	${FREETYPE2_LIBDIR}
)


target_link_libraries(workbench PRIVATE
	wgsoftgfx
	wgopenglgfx
	wgfreetypefont
	wondergui 
	${SDL2_LIBS}
	${SDL2IMAGE_LIBS}
	${OPENGL_LIBS}
	${FREETYPE2_LIB}
)

