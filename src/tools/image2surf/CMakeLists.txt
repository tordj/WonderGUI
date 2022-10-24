set(CMAKE_CXX_STANDARD 20)

source_group(Files REGULAR_EXPRESSION ".*")

add_executable(image2surf

	main.cpp
	stb_image.h
)

target_working_directory(image2surf ${WG_ROOT} )

target_link_libraries(image2surf PRIVATE
	wgsoftgfx
	wondergui 
)


