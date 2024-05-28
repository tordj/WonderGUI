set(CMAKE_CXX_STANDARD 20)


set( SOURCES
	layerworkbench.cpp
	layerworkbench.h
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

add_executable(layerworkbench
	${SOURCES}
)

target_include_directories(layerworkbench PRIVATE 
	${WAPP_INCDIRS}
)

target_working_directory(layerworkbench ${WG_ROOT} )

target_link_libraries(layerworkbench PRIVATE
	${WAPPLIB}
)

