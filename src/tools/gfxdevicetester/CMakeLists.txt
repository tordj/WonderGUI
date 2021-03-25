
add_executable(gfxdevicetester

	framework_sdlgl/main.cpp
	app.h
	device.cpp
	device.h
	gfxdevicetester.cpp
	gfxdevicetester.h
	testunit.h
	
	testsuites/a8tests.h
	testsuites/blendtests.h
	testsuites/blitblendtests.h
	testsuites/blitconsistencytest.h
	testsuites/blittests.h
	testsuites/canvasformattests.h
	testsuites/clutblittests.h
	testsuites/filltests.h
	testsuites/linetests.h
	testsuites/mipmaptests.h
	testsuites/piecharttests.h
	testsuites/plottests.h
	testsuites/segmenttests.h
	testsuites/testsuite.h
	testsuites/tiletests.h
	testsuites/tintblittests.h
	testsuites/tintsegmenttests.h
	testsuites/wavetests.h
)

target_link_directories(gfxdevicetester PRIVATE 
)


if(WIN32)
	target_link_libraries(gfxdevicetester PRIVATE
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

	set_property(TARGET gfxdevicetester PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/..)

else()
	target_link_libraries(gfxdevicetester PRIVATE
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



