set(CMAKE_CXX_STANDARD 20)

source_group(Files REGULAR_EXPRESSION ".*")

add_executable(unittester

	main.cpp
	testframework.cpp
	testframework.h
	tests/gfxstream.cpp
	tests/gfxstream.h
	tests/softgfxdevice.cpp
	tests/softgfxdevice.h
)

target_working_directory(unittester ${WG_ROOT} )


target_include_directories(unittester PRIVATE 
	${WONDERGUI_INCDIRS}
	${WGSOFTGFX_INCDIRS}
	${WGFREETYPEFONT_INCDIRS}
)

target_link_libraries(unittester PRIVATE
	wgfreetypefont
	wondergui
	wgsoftgfx
)

