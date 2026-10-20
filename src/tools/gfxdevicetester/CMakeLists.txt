
set(CMAKE_CXX_STANDARD 20)


set( SOURCES

	device.cpp
	device.h
	gfxdevicetester.cpp
	gfxdevicetester.h
	testunit.h
)

# The test suites live with gfxdevicetester2, which is replacing this tool.

set( TESTSUITE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../gfxdevicetester2/testsuites )

set( TESTSUITES

	${TESTSUITE_DIR}/a8tests.h
	${TESTSUITE_DIR}/blurtests.h
	${TESTSUITE_DIR}/blursourcetests.h
	${TESTSUITE_DIR}/blendfixedcolortests.h
	${TESTSUITE_DIR}/blendtests.h
	${TESTSUITE_DIR}/blitblendtests.h
	${TESTSUITE_DIR}/blitconsistencytest.h
	${TESTSUITE_DIR}/blittests.h
	${TESTSUITE_DIR}/canvasformattests.h
	${TESTSUITE_DIR}/canvaslayertests.h
	${TESTSUITE_DIR}/colortests.h
	${TESTSUITE_DIR}/edgemaptests.h
	${TESTSUITE_DIR}/edgemaptinttests.h
	${TESTSUITE_DIR}/roundedboxtests.h
	${TESTSUITE_DIR}/ringtests.h
	${TESTSUITE_DIR}/edgemapboxtests.h
	${TESTSUITE_DIR}/edgemapringtests.h
	${TESTSUITE_DIR}/edgemapwavetests.h
	${TESTSUITE_DIR}/filltests.h
	${TESTSUITE_DIR}/linetests.h
	${TESTSUITE_DIR}/mipmaptests.h
	${TESTSUITE_DIR}/paletteblittests.h
	${TESTSUITE_DIR}/palettecapacitytests.h
	${TESTSUITE_DIR}/piecharttests.h
	${TESTSUITE_DIR}/readbacktests.h
	${TESTSUITE_DIR}/rgb565bigendiantests.h
	${TESTSUITE_DIR}/samplingtests.h
	${TESTSUITE_DIR}/segmenttests.h
	${TESTSUITE_DIR}/testsuite.h
	${TESTSUITE_DIR}/tiletests.h
	${TESTSUITE_DIR}/tintblittests.h
	${TESTSUITE_DIR}/tintsegmenttests.h
	${TESTSUITE_DIR}/tinttests.h
	${TESTSUITE_DIR}/wavetests.h
)


source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})
source_group("testsuites" FILES ${TESTSUITES})

add_executable(gfxdevicetester
	${SOURCES}
	${TESTSUITES}
)


target_include_directories(gfxdevicetester PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/../gfxdevicetester2
	${WAPP_INCDIRS}
	${WGOPENGLGFX_INCDIRS}
	${WGMETALGFX_INCDIRS}
	${WGSOFTGFX_INCDIRS}
	${WGGFXSTREAM_INCDIRS}
)

target_link_directories(gfxdevicetester PRIVATE 
)

target_working_directory(gfxdevicetester ${WG_ROOT} )

target_link_libraries(gfxdevicetester PRIVATE
	${WAPPLIB}
	wgsoftgfx
	wggfxstream
)

