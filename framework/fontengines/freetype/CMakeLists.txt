
add_library( wgfreetypefont 

    wg3_freetypefont.h
    wg3_freetypefont.cpp
)

if(WIN32)
    target_include_directories(wgfreetypefont PRIVATE ../../../freetype/include )
    target_link_directories(wgfreetypefont PRIVATE ../freetype/lib/x64 )
elseif(APPLE)
    target_include_directories(wgfreetypefont PRIVATE ../../../freetype/include )
    target_link_directories(wgfreetypefont PRIVATE ../freetype/lib)
else()
    target_include_directories(wgfreetypefont PRIVATE /usr/include/freetype2 )
endif()

if(APPLE)
    target_link_libraries(wgfreetypefont PRIVATE freetype bz2 z )
else()
    target_link_libraries(wgfreetypefont PRIVATE freetype )
endif()
