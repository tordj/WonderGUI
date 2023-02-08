### This tool uses SDL2_image for generating PNG-files. Can only be built if
### SDL2_image development libs and headers are available and linked against
###a SDL2-based WonderApp framework.

set(CMAKE_CXX_STANDARD 20)

source_group(Files REGULAR_EXPRESSION ".*")

add_executable(fontgenerator
	fontgenerator.cpp
	fontgenerator.h
)

target_include_directories(fontgenerator PRIVATE 
	${WAPP_INCDIRS}
	${SDL2_INCDIR}
	${SDL2IMAGE_INCDIR}
)

target_working_directory(fontgenerator ${WG_ROOT} )

target_link_libraries(fontgenerator PRIVATE
	${WAPPLIB}
)

