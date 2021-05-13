
source_group(Files REGULAR_EXPRESSION ".*")

add_executable(gfxworkbench

	main.cpp
)

target_link_directories(gfxworkbench PRIVATE 
)

target_working_directory(gfxworkbench ${WG_ROOT})

target_link_libraries(gfxworkbench PRIVATE
	wgsoftgfx
	wgopenglgfx
	wgfreetypefont
	wondergui 
	${SDL2_LIBS}
	${SDL2IMAGE_LIBS}
	${OPENGL_LIBS}
)
