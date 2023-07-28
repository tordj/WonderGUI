set(CMAKE_CXX_STANDARD 20)

set( SOURCES

	streamanalyzer.cpp
	streamanalyzer.h
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

add_executable(streamanalyzer
	${SOURCES}
)

target_include_directories(streamanalyzer PRIVATE 
	${WAPP_INCDIRS}
	${GFXSTREAM_INCDIRS}
	${WGSOFTGFX_INCDIRS}
)

target_working_directory(streamanalyzer ${WG_ROOT} )

target_link_libraries(streamanalyzer PRIVATE
	${WAPPLIB}
	wggfxstream
)


