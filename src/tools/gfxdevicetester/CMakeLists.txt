
set(CMAKE_CXX_STANDARD 20)


if(WIN32)

	set(SDL2_LIBS SDL2 SDL2main)
	set(SDL2IMAGE_LIBS SDL2_image)

	set(OPENGL_LIBS glew32 opengl32)

elseif(APPLE)

	find_library(SDL2_LIBS SDL2)
	find_library(SDL2IMAGE_LIBS SDL2_image)

	find_library(OPENGL_LIBS OpenGL)

else()

	set(SDL2_LIBS SDL2)
	set(SDL2IMAGE_LIBS SDL2_image)

	set(OPENGL_LIBS GLEW GL)
endif()

source_group(Files REGULAR_EXPRESSION ".*")
source_group(Files/TestSuites REGULAR_EXPRESSION "testsuites/*")


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
	testsuites/canvaslayertests.h
	testsuites/clutblittests.h
	testsuites/filltests.h
	testsuites/linetests.h
	testsuites/mipmaptests.h
	testsuites/piecharttests.h
	testsuites/plottests.h
	testsuites/rgb565bigendiantests.h	
	testsuites/segmenttests.h
	testsuites/testsuite.h
	testsuites/tiletests.h
	testsuites/tintblittests.h
	testsuites/tintsegmenttests.h
	testsuites/wavetests.h
)

target_link_directories(gfxdevicetester PRIVATE 
)

target_working_directory(gfxdevicetester ${WG_ROOT} )

target_link_libraries(gfxdevicetester PRIVATE
	wgsoftgfx
	wgopenglgfx
	wgstreamgfx
	wgcabiwrapper
	wgfreetypefont
	wondergui 
	${SDL2_LIBS}
	${SDL2IMAGE_LIBS}
	${OPENGL_LIBS}
)

if(APPLE)

	set(compileRes
		framework_metal/Base.lproj/Main.storyboard
		framework_metal/Assets.xcassets
	)


	add_executable(gfxdevicetester_metal MACOSX_BUNDLE

		framework_metal/AppDelegate.mm
		framework_metal/GameViewController.mm
		framework_metal/Renderer.mm
		framework_metal/main.m
		${compileRes}

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

	find_library(APPKIT AppKit)
	find_library(METAL Metal)


	target_link_directories(gfxdevicetester_metal PRIVATE 
	)

	target_working_directory(gfxdevicetester_metal ${WG_ROOT} )

	target_link_libraries(gfxdevicetester_metal PRIVATE
		wgsoftgfx
		wgmetalgfx
		wgstreamgfx
		wgcabiwrapper
		wgfreetypefont
		wondergui
		${APPKIT}
		${METAL}
	)

	set_target_properties(gfxdevicetester_metal PROPERTIES

		MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/framework_metal/Info.plist"

	)

	set_source_files_properties(${compileRes} PROPERTIES
		MACOSX_PACKAGE_LOCATION Resources
	)

endif()
