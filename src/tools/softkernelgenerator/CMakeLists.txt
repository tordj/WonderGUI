set(CMAKE_CXX_STANDARD 20)

source_group(Files REGULAR_EXPRESSION ".*")

add_executable(softkernelgenerator

	softkernelgenerator.cpp
	softkernelgenerator.h
	kerneldb.cpp
	kerneldb.h
)

target_include_directories(softkernelgenerator PRIVATE 
	../../wonderapp/include
)

target_working_directory(softkernelgenerator ${WG_ROOT} )

target_link_libraries(softkernelgenerator PRIVATE
	${WAPPLIB}
)


