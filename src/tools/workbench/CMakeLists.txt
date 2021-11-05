
set(CMAKE_CXX_STANDARD 20)

source_group(Files REGULAR_EXPRESSION ".*")

add_executable(workbench

	main.cpp
)

target_link_directories(workbench PRIVATE 
)

target_working_directory(workbench ${WG_ROOT} )

target_link_libraries(workbench PRIVATE
	wgsoftgfx
	wgopenglgfx
	wgfreetypefont
	wondergui 
	${SDL2_LIBS}
	${SDL2IMAGE_LIBS}
	${OPENGL_LIBS}
)

