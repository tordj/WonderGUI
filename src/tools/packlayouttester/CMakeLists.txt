set(CMAKE_CXX_STANDARD 20)

source_group(Files REGULAR_EXPRESSION ".*")

add_executable(packlayouttester
	packlayouttester.cpp
	packlayouttester.h
)

target_include_directories(packlayouttester PRIVATE 
	../../wonderapp/include
)

target_working_directory(packlayouttester ${WG_ROOT} )

target_link_libraries(packlayouttester PRIVATE
	${WAPPLIB}
)

