source_group(Files REGULAR_EXPRESSION ".*")

add_library( wgfreetypefont STATIC

    wg_freetypefont.h
    wg_freetypefont.cpp
    wg_c_freetypefont.h
    wg_c_freetypefont.cpp
)

target_include_directories(wgfreetypefont PRIVATE
    ${WGFREETYPEFONT_INCDIRS}
    ${WONDERGLYPHS_INCDIRS}
)

if(WIN32)
    target_include_directories(wgfreetypefont PRIVATE ../../../freetype/include )
    target_link_directories(wgfreetypefont PRIVATE ../freetype/lib/x64 )
elseif(APPLE)
    target_include_directories(wgfreetypefont PRIVATE ../../../freetype/include )
    target_link_directories(wgfreetypefont PRIVATE ../../../freetype/lib)
else()
    target_include_directories(wgfreetypefont PRIVATE /usr/include/freetype2 )
endif()

if(APPLE)
    target_link_libraries(wgfreetypefont PRIVATE freetype_arm64 freetype_x86_64 bz2 z )
else()
    target_link_libraries(wgfreetypefont PRIVATE freetype )
endif()
