set(CMAKE_CXX_STANDARD 20)

source_group(Files REGULAR_EXPRESSION ".*")

add_executable(texteditor

	texteditor.cpp
	texteditor.h
	editorwindow.cpp
	editorwindow.h
)

target_include_directories(texteditor PRIVATE 
	${WAPP_INCDIRS}
)

target_working_directory(texteditor ${WG_ROOT} )

target_link_libraries(texteditor PRIVATE
	${WAPPLIB}
)


