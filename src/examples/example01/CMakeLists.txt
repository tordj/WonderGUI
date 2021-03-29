
add_executable(example01

	example01.cpp
)

target_link_directories(example01 PRIVATE 
	../../SDL2/lib/x64
)


if(WIN32)
	target_link_libraries(example01 PRIVATE 
		wgsoftgfx
		wondergui 
		SDL2
		SDL2main
	)

	set_property(TARGET example01 PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/..)
elseif(APPLE)

    find_library(FRAMEWORK_SDL2 SDL2)

    target_link_libraries(example01 PRIVATE
        wgsoftgfx
        wondergui
        ${FRAMEWORK_SDL2}
    )


else()
	target_link_libraries(example01 PRIVATE 
		wgsoftgfx
		wondergui 
		SDL2
	)

endif()


