
set(CMAKE_CXX_STANDARD 20)


source_group(Files REGULAR_EXPRESSION ".*")
source_group(Files/TestSuites REGULAR_EXPRESSION "testsuites/*")


add_executable(gfxdevicetester

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
	testsuites/filltests.h
	testsuites/linetests.h
	testsuites/mipmaptests.h
	testsuites/paletteblittests.h
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

target_include_directories(gfxdevicetester PRIVATE
	${WAPP_INCDIRS}
)

target_link_directories(gfxdevicetester PRIVATE 
)

target_working_directory(gfxdevicetester ${WG_ROOT} )

target_link_libraries(gfxdevicetester PRIVATE
	${WAPPLIB}
	wgsoftgfx
)

