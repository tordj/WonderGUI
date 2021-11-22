
add_executable(workbench

	main.cpp
	../extras/sdl_wglib.cpp
	../extras/sdl_wglib.h

)

target_link_directories(workbench PRIVATE 
)

target_working_directory(workbench ${WG_ROOT} )

target_link_libraries(workbench PRIVATE
	wgsoftgfx
#	wgopenglgfx
	wgfreetypefont
	wondergui 
	${SDL2_LIBS}
	${SDL2IMAGE_LIBS}
	${OPENGL_LIBS}
)

