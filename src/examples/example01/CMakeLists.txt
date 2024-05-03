set(CMAKE_CXX_STANDARD 20)

set(SOURCES
	example01.cpp
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

add_executable(example01 MACOSX_BUNDLE
	${SOURCES} 
)

target_include_directories(example01 PRIVATE

	${SDL2_INCDIR}
)


target_link_directories(example01 PRIVATE 
	${SDL2_LIBDIR}
)

target_include_directories(example01 PRIVATE 
	${WONDERGUI_INCDIRS}
	${WGSOFTGFX_INCDIRS}
)


target_working_directory(example01 ${WG_ROOT} )

target_link_libraries(example01 PRIVATE
	wgsoftgfx
	wondergui 
	${SDL2_LIBS}
)

set_target_properties(example01 PROPERTIES

    XCODE_ATTRIBUTE_PRODUCT_NAME "example01"
    XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.wondergui.example01"

    XCODE_ATTRIBUTE_MARKETING_VERSION "1.0.0"
    XCODE_ATTRIBUTE_CURRENT_PROJECT_VERSION "1.0.0"
    XCODE_ATTRIBUTE_GENERATE_INFOPLIST_FILE YES
    XCODE_ATTRIBUTE_INFOPLIST_FILE  ""
    
    XCODE_ATTRIBUTE_INFOPLIST_KEY_CFBundleDisplayName "WonderGUI Example 1"
    XCODE_ATTRIBUTE_INFOPLIST_KEY_LSApplicationCategoryType "public.app-category.games"
    XCODE_ATTRIBUTE_INFOPLIST_KEY_NSPrincipalClass  "NSApplication"
    XCODE_ATTRIBUTE_INFOPLIST_KEY_NSMainStoryboardFile "Main"
    XCODE_ATTRIBUTE_INFOPLIST_KEY_NSHumanReadableCopyright "(c) 2024 Nordic UI Technologies AB"
    
    XCODE_EMBED_FRAMEWORKS          ${WG_ROOT}/SDL2.framework
    XCODE_EMBED_FRAMEWORKS_CODE_SIGN_ON_COPY FALSE
    XCODE_EMBED_FRAMEWORKS_REMOVE_HEADERS_ON_COPY TRUE
 
    XCODE_ATTRIBUTE_LD_RUNPATH_SEARCH_PATHS "@executable_path/../Frameworks"
)
