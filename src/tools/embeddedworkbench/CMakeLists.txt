set(CMAKE_CXX_STANDARD 11)

source_group(Files REGULAR_EXPRESSION ".*")



# target_compile_options(wg-display PRIVATE -fno-exceptions -fno-rtti)



add_executable(embeddedworkbench

	cpp_necessities.cpp
	cpp_necessities.h
	main.c
)

target_compile_options(embeddedworkbench PRIVATE -lm -lstd=c99)

target_working_directory(embeddedworkbench ${CMAKE_CURRENT_LIST_DIR} )

target_include_directories(embeddedworkbench PRIVATE
	${WONDERGLYPHS_INCDIRS}
	${WGSOFTGFX_INCDIRS}
	${SDL2_INCDIR}
)

target_link_directories(embeddedworkbench PRIVATE 
	${SDL2_LIBDIR}
)

target_link_libraries(embeddedworkbench PRIVATE

	wonderglyphs
	wgsoftgfx	
	${SDL2_LIBS}
)



