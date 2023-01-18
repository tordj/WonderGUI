set(CMAKE_CXX_STANDARD 20)

source_group(Files REGULAR_EXPRESSION ".*")

add_library(cabiclient SHARED
	cabiclient.cpp
	
	)

target_link_libraries(cabiclient PRIVATE
	wondergui
	wgfreetypefont
	wgcabiclient
)

set_target_properties(cabiclient PROPERTIES CXX_VISIBILITY_PRESET hidden)

add_executable(cabiworkbench

	cabiworkbench.cpp
	cabiworkbench.h
)

target_include_directories(cabiworkbench PRIVATE 
	../../wonderapp/include
)

add_dependencies(cabiworkbench cabiclient)

target_working_directory(cabiworkbench ${WG_ROOT} )

target_link_libraries(cabiworkbench PRIVATE
	${WAPPLIB}
	wgcabihost
)


