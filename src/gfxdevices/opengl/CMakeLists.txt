
add_library( wgopenglgfx 

	wg_glgfxdevice.cpp
	wg_glgfxdevice.h
	wg_glshaders.cpp
	wg_glsurface.cpp
	wg_glsurface.h
	wg_glsurfacefactory.cpp
	wg_glsurfacefactory.h
)

target_include_directories(wgopenglgfx PRIVATE 
	
)

