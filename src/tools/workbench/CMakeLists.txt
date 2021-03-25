
add_executable(workbench

	main.cpp
)

target_link_directories(workbench PRIVATE 
)


if(WIN32)
	target_link_libraries(workbench PRIVATE
		wgsoftgfx
		wgopenglgfx
		wgfreetypefont
		wondergui 
		SDL2
		SDL2main
		SDL2_image
		glew32
		opengl32
	)

	set_property(TARGET workbench PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/..)

else()
	target_link_libraries(workbench PRIVATE
		wgsoftgfx
		wgopenglgfx
		wgfreetypefont
		wondergui 
		SDL2
		SDL2_image
		GLEW
		GL
	)

endif()


