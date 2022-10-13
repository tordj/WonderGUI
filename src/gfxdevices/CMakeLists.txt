
add_subdirectory(software)
add_subdirectory(stream)
add_subdirectory(opengl)
if(APPLE)
	add_subdirectory(metal)
	set_target_properties( wgmetalgfx PROPERTIES FOLDER "Tools" )
endif()

set_target_properties( wgsoftgfx wgopenglgfx wgstreamgfx PROPERTIES FOLDER "Graphics Devices" )
