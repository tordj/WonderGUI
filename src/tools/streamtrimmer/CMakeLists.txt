set(CMAKE_CXX_STANDARD 20)



set( SOURCES

	main.cpp
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

add_executable(streamtrimmer
	${SOURCES}
)


target_link_directories(streamtrimmer PRIVATE 
)

target_include_directories(streamtrimmer PRIVATE 
	${WONDERGFX_INCDIRS}
	${GFXSTREAM_INCDIRS}
)


target_working_directory(streamtrimmer ${WG_ROOT} )

target_link_libraries(streamtrimmer PRIVATE
	wondergfx
	wggfxstream
)


