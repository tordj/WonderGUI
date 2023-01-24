set(CMAKE_CXX_STANDARD 20)

if(WIN32)
	set(SDL2_LIBS SDL2 SDL2main)
elseif(APPLE)
	find_library(SDL2_LIBS SDL2)
else()
	set(SDL2_LIBS SDL2)
endif()


source_group(Files REGULAR_EXPRESSION ".*")

add_executable(example01

	example01.cpp
)

target_link_directories(example01 PRIVATE 
	../../SDL2/lib/x64
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
