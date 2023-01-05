set(CMAKE_CXX_STANDARD 20)

source_group(Files REGULAR_EXPRESSION ".*")

add_executable(surfviewer

	surfviewer.cpp
	surfviewer.h
)

target_include_directories(surfviewer PRIVATE 
	../../wonderapp/include
)

target_working_directory(surfviewer ${WG_ROOT} )

target_link_libraries(surfviewer PRIVATE
	${WAPPLIB}
)


