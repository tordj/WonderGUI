set(CMAKE_CXX_STANDARD 20)

set( SOURCES

	streamplayer.cpp
	streamplayer.h
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

add_executable(streamplayer
	${SOURCES}
)

target_include_directories(streamplayer PRIVATE 
	${WAPP_INCDIRS}
	${GFXSTREAM_INCDIRS}
	${WGSOFTGFX_INCDIRS}
)

target_working_directory(streamplayer ${WG_ROOT} )

target_link_libraries(streamplayer PRIVATE
	${WAPPLIB}
	wgsoftgfx
	wggfxstream
)


