
add_executable(unittester

	main.cpp
	testframework.cpp
	testframework.h
	text_test.cpp
	text_test.h
	valueformat_test.cpp
	valueformat_test.h
)


if(WIN32)
	target_link_libraries(unittester PRIVATE
		wgfreetypefont
		wondergui 
	)

	set_property(TARGET unittester PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/..)

else()
	target_link_libraries(unittester PRIVATE
		wgfreetypefont
		wondergui 
	)

endif()


