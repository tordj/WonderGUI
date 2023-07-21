set(CMAKE_CXX_STANDARD 20)

source_group(Files REGULAR_EXPRESSION ".*")

add_executable(streamanalyzer

	streamanalyzer.cpp
	streamanalyzer.h
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


