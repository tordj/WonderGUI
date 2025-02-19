
set(SOURCES

	c-api/wg_c_softgfx.cpp
	c-api/wg_c_softgfx.h
	c-api/wg_c_lineargfx.cpp
	c-api/wg_c_lineargfx.h
	c-api/wg_c_softkernels_default.h
	c-api/wg_c_softkernels_tinylinear.h
	c-api/wg_c_softkernels_rgb555be_base.h
	c-api/wg_c_softkernels_rgb555be_extras.h
	c-api/wg_c_softkernels_rgb565be_base.h
	c-api/wg_c_softkernels_rgb565be_extras.h

	wg_softkernels_default.cpp
	wg_softkernels_default.h
	wg_softkernels_tinylinear.cpp
	wg_softkernels_tinylinear.h

	wg_softkernels_bgr565srgb_base.cpp
	wg_softkernels_bgr565srgb_base.h
	wg_softkernels_bgr565srgb_extras.cpp
	wg_softkernels_bgr565srgb_extras.h

	wg_softkernels_rgb555be_base.cpp
	wg_softkernels_rgb555be_base.h
	wg_softkernels_rgb555be_extras.cpp
	wg_softkernels_rgb555be_extras.h
	wg_softkernels_rgb565be_base.cpp
	wg_softkernels_rgb565be_base.h
	wg_softkernels_rgb565be_extras.cpp
	wg_softkernels_rgb565be_extras.h
	wg_softkernelstanza.impl.h
	wg_softgfxdevice.cpp
	wg_softgfxdevice.h
	wg_softsurface.cpp
	wg_softsurface.h
	wg_softsurfacefactory.cpp
	wg_softsurfacefactory.h
	wg_softedgemap.cpp
	wg_softedgemap.h
	wg_softedgemapfactory.cpp
	wg_softedgemapfactory.h

	wg_lineargfxdevice.cpp
	wg_lineargfxdevice.h

)

set(INCDIRS
	${CMAKE_CURRENT_LIST_DIR}
	${CMAKE_CURRENT_LIST_DIR}/c-api
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

add_library( wgsoftgfx STATIC ${SOURCES} )

target_include_directories(wgsoftgfx PRIVATE 
	${WONDERGFX_INCDIRS}
	${INCDIRS}
)

set_target_properties( wgsoftgfx PROPERTIES FOLDER "Extensions" )


list(TRANSFORM SOURCES PREPEND ${CMAKE_CURRENT_LIST_DIR}/ )
set(WGSOFTGFX_SOURCES ${SOURCES} PARENT_SCOPE)
set(WGSOFTGFX_INCDIRS ${INCDIRS} PARENT_SCOPE)
