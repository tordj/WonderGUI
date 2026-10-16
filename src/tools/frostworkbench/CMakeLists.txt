set(CMAKE_CXX_STANDARD 20)


set( SOURCES
	frostworkbench.cpp
	frostworkbench.h
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

add_executable(frostworkbench
	${SOURCES}
)

target_include_directories(frostworkbench PRIVATE 
	${WAPP_INCDIRS}
)

target_working_directory(frostworkbench ${WG_ROOT} )

target_link_libraries(frostworkbench PRIVATE
	${WAPPLIB}
)
