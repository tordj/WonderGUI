project(FreetypeFont)

ADD_LIBRARY( wgfreetypefont STATIC 

	wg_freetypefont.cpp
)

target_include_directories(wgsoftgfx PRIVATE 
	../../freetype/include
)

