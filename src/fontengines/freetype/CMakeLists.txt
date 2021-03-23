project(FreetypeFont)

ADD_LIBRARY( wgfreetypefont STATIC 

	wg_freetypefont.cpp
)

target_include_directories(wgfreetypefont PRIVATE 
	/usr/include/freetype2
	../../freetype/include
)
