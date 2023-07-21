set(CMAKE_CXX_STANDARD 20)


source_group(Files REGULAR_EXPRESSION ".*")

add_executable(streamtrimmer

	main.cpp
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


