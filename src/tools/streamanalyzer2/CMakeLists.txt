set(CMAKE_CXX_STANDARD 20)

source_group(Files REGULAR_EXPRESSION ".*")

add_executable(streamanalyzer2

	streamanalyzer.cpp
	streamanalyzer.h
)

target_include_directories(streamanalyzer2 PRIVATE 
	../../wonderapp/include
)

target_working_directory(streamanalyzer2 ${WG_ROOT} )

target_link_libraries(streamanalyzer2 PRIVATE
	${WAPPLIB}
)


