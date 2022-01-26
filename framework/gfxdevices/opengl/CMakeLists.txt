source_group(Files REGULAR_EXPRESSION ".*")

add_library( wgopenglgfx 

	wg3_glgfxdevice.cpp
	wg3_glgfxdevice.h
	wg3_glshaders.cpp
	wg3_glsurface.cpp
	wg3_glsurface.h
	wg3_glsurfacefactory.cpp
	wg3_glsurfacefactory.h
)

target_include_directories(wgopenglgfx PRIVATE 
	
)

