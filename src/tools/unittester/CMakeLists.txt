
add_executable(unittester

	main.cpp
	testframework.cpp
	testframework.h
	text_test.cpp
	text_test.h
	valueformat_test.cpp
	valueformat_test.h
)

target_working_directory(unittester, WG_ROOT)

target_link_libraries(unittester PRIVATE
	wgfreetypefont
	wondergui 
)

