set(CMAKE_CXX_STANDARD 20)

source_group(Files REGULAR_EXPRESSION ".*")

add_executable(fontgenerator
	fontgenerator.cpp
	fontgenerator.h
)

target_include_directories(fontgenerator PRIVATE 
	../../wonderapp/include
)

target_working_directory(fontgenerator ${WG_ROOT} )

target_link_libraries(fontgenerator PRIVATE
	${WAPPLIB}
)


