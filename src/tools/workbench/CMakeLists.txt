
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

elseif(APPLE)
    find_library(FRAMEWORK_SDL2 SDL2)
    find_library(FRAMEWORK_SDL2_image SDL2_image)
    find_library(FRAMEWORK_OPENGL OpenGL)

    target_link_libraries(workbench PRIVATE
        wgsoftgfx
        wgopenglgfx
        wgfreetypefont
        wondergui
        ${FRAMEWORK_SDL2}
        ${FRAMEWORK_SDL2_image}
        ${FRAMEWORK_OPENGL}
    )

    set_target_properties(workbench PROPERTIES
        XCODE_GENERATE_SCHEME TRUE
        XCODE_SCHEME_WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/.."
    )

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


