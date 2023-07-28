set(CMAKE_CXX_STANDARD 20)

set(SOURCES
	example01.cpp
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

add_executable(example01 
	${SOURCES} 
)

target_include_directories(example01 PRIVATE

	${SDL2_INCDIR}
)


target_link_directories(example01 PRIVATE 
	${SDL2_LIBDIR}
)

target_include_directories(example01 PRIVATE 
	${WONDERGUI_INCDIRS}
	${WGSOFTGFX_INCDIRS}
)


target_working_directory(example01 ${WG_ROOT} )

target_link_libraries(example01 PRIVATE
	wgsoftgfx
	wondergui 
	${SDL2_LIBS}
)
