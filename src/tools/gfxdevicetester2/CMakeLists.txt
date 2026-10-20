
set(CMAKE_CXX_STANDARD 20)


set( SOURCES

	gfxdevicetester2.cpp
	gfxdevicetester2.h

	testsuites/a8tests.h
	testsuites/blurtests.h
	testsuites/blursourcetests.h
	testsuites/blendfixedcolortests.h
	testsuites/blendtests.h
	testsuites/blitblendtests.h
	testsuites/blitconsistencytest.h
	testsuites/blittests.h
	testsuites/canvasformattests.h
	testsuites/canvaslayertests.h
	testsuites/colortests.h
	testsuites/edgemaptests.h
	testsuites/edgemaptinttests.h
	testsuites/roundedboxtests.h
	testsuites/ringtests.h
	testsuites/warptests.h
	testsuites/linecaptests.h
	testsuites/edgemapboxtests.h
	testsuites/edgemapringtests.h
	testsuites/edgemapwavetests.h
	testsuites/filltests.h
	testsuites/flattenlayerstests.h
	testsuites/layerroundtriptests.h
	testsuites/blendruletests.h
	testsuites/linetests.h
	testsuites/mipmaptests.h
	testsuites/paletteblittests.h
	testsuites/palettecapacitytests.h
	testsuites/piecharttests.h
	testsuites/readbacktests.h
	testsuites/rgb565bigendiantests.h
	testsuites/samplingtests.h
	testsuites/segmenttests.h
	testsuites/testsuite.h
	testsuites/tiletests.h
	testsuites/tintblittests.h
	testsuites/tintsegmenttests.h
	testsuites/tinttests.h
	testsuites/wavetests.h
)


source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

add_executable(gfxdevicetester2
	${SOURCES}
)


target_include_directories(gfxdevicetester2 PRIVATE
	${WAPP_INCDIRS}
	${WGSOFTGFX_INCDIRS}
)

target_working_directory(gfxdevicetester2 ${WG_ROOT} )

target_link_libraries(gfxdevicetester2 PRIVATE
	${WAPPLIB}
	wgsoftgfx
)
