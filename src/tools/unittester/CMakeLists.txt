set(CMAKE_CXX_STANDARD 20)

set( SOURCES

	main.cpp
	testframework.cpp
	testframework.h
	tests/gfxstream.cpp
	tests/gfxstream.h
	tests/softgfxdevice.cpp
	tests/softgfxdevice.h
	tests/slotlinks.cpp
	tests/slotlinks.h
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

add_executable(unittester
	${SOURCES}
)

target_working_directory(unittester ${WG_ROOT} )


target_include_directories(unittester PRIVATE 
	${WONDERGUI_INCDIRS}
	${WGSOFTGFX_INCDIRS}
	${GFXSTREAM_INCDIRS}
	${WGFREETYPEFONT_INCDIRS}
)

target_link_libraries(unittester PRIVATE
	wgfreetypefont
	wondergui
	wgsoftgfx
	wggfxstream
)

