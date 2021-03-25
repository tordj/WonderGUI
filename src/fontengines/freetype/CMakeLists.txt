
add_library( wgfreetypefont 

wg_freetypefont.h
wg_freetypefont.cpp
)

target_include_directories(wgfreetypefont PRIVATE 
	/usr/include/freetype2
	../../../freetype/include
)

target_link_libraries(wgfreetypefont
	PRIVATE freetype
)