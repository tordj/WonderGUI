
project(workbench)

add_executable(workbench

	main.cpp
)

target_link_directories(workbench PRIVATE 
	../../SDL2/lib/x64
)


if(WIN32)
target_link_libraries(workbench PUBLIC 
	wgsoftgfx
	wgopenglgfx
	wgfreetypefont
	wondergui 
	SDL2
	SDL2main
	SDL2_image
	GLEW
	GL
	freetype
)

else()
target_link_libraries(workbench PUBLIC 
	wgsoftgfx
	wgopenglgfx
	wgfreetypefont
	wondergui 
	SDL2
	SDL2_image
	GLEW
	GL
	freetype
)

endif()


