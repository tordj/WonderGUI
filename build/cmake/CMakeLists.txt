cmake_minimum_required(VERSION 3.10)

project(wondergui VERSION 3.0 DESCRIPTION "WonderGUI User Interface Library")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)


include_directories( 
	../../src
	../../src/base
	../../src/components
	../../src/sizebrokers
	../../src/skins
	../../src/skins/anim
	../../src/skins/meter
	../../src/skins/misc
	../../src/skins/state
	../../src/skins/static
	../../src/textmappers
	../../src/valueformatters
	../../src/widgets
	../../src/widgets/capsules
	../../src/widgets/layers
	../../src/widgets/lists
	../../src/widgets/panels
)


##############################
### WONDERGUI BASE LIBRARY ###
##############################

ADD_LIBRARY( wondergui STATIC
	../../src/base/wg_base.cpp
	../../src/base/wg_bitmapfont.cpp
	../../src/base/wg_blob.cpp
	../../src/base/wg_canvaslayers.cpp
	../../src/base/wg_caret.cpp
	../../src/base/wg_chain.cpp
	../../src/base/wg_charbuffer.cpp
	../../src/base/wg_charseq.cpp
	../../src/base/wg_codepages.cpp
	../../src/base/wg_color.cpp
	../../src/base/wg_context.cpp
	../../src/base/wg_dataset.cpp
	../../src/base/wg_dummyfont.cpp
	../../src/base/wg_enumextras.cpp
	../../src/base/wg_font.cpp
	../../src/base/wg_geo.cpp
	../../src/base/wg_gfxdevice.cpp
	../../src/base/wg_gfxstreamlogger.cpp
	../../src/base/wg_gfxstreamplayer.cpp
	../../src/base/wg_gfxstreamplug.cpp
	../../src/base/wg_gfxstreamreader.cpp
	../../src/base/wg_gfxstreamwriter.cpp
	../../src/base/wg_gradient.cpp
	../../src/base/wg_inputhandler.cpp
	../../src/base/wg_mempool.cpp
	../../src/base/wg_memstack.cpp
	../../src/base/wg_msg.cpp
	../../src/base/wg_msglogger.cpp
	../../src/base/wg_msgrouter.cpp
	../../src/base/wg_mu.cpp
	../../src/base/wg_nullgfxdevice.cpp
	../../src/base/wg_object.cpp
	../../src/base/wg_patches.cpp
	../../src/base/wg_receiver.cpp
	../../src/base/wg_resdb.cpp
	../../src/base/wg_scrollbartarget.cpp
	../../src/base/wg_skinslotmanager.cpp
	../../src/base/wg_slot.cpp
	../../src/base/wg_string.cpp
	../../src/base/wg_surface.cpp
	../../src/base/wg_surfacefactory.cpp
	../../src/base/wg_textlink.cpp
	../../src/base/wg_textstyle.cpp
	../../src/base/wg_textstylemanager.cpp
	../../src/base/wg_texttool.cpp
	../../src/base/wg_togglegroup.cpp
	../../src/base/wg_util.cpp			

	../../src/components/wg_canimframes.cpp
	../../src/components/wg_ccanvas.cpp
	../../src/components/wg_ccolumnheader.cpp
	../../src/components/wg_cdynamicslotcollection.cpp
	../../src/components/wg_cgfxinstream.cpp
	../../src/components/wg_cgfxoutstream.cpp
	../../src/components/wg_cicondisplay.cpp
	../../src/components/wg_component.cpp
	../../src/components/wg_cshadows.cpp
	../../src/components/wg_cskinslot.cpp
	../../src/components/wg_cskinslotmi.cpp
	../../src/components/wg_cslot.cpp
	../../src/components/wg_cstandardslot.cpp
	../../src/components/wg_cstaticslotcollection.cpp
	../../src/components/wg_cstatictextdisplay.cpp
	../../src/components/wg_cstaticvaluedisplay.cpp
	../../src/components/wg_ctext.cpp
	../../src/components/wg_ctextdisplay.cpp
	../../src/components/wg_ctexteditor.cpp
	../../src/components/wg_cvaluedisplay.cpp
	../../src/components/wg_cvalueeditor.cpp
	../../src/components/wg_geocomponent.cpp

	../../src/sizebrokers/wg_sizebroker.cpp
	../../src/sizebrokers/wg_uniform_sizebroker.cpp
	../../src/sizebrokers/wg_scalepreferred_sizebroker.cpp

	../../src/skins/wg_animskin.cpp
	../../src/skins/wg_skin.cpp
	../../src/skins/wg_stateskin.cpp

	../../src/skins/anim/wg_spinanimskin.cpp
	../../src/skins/meter/wg_fillmeterskin.cpp
	../../src/skins/meter/wg_framemeterskin.cpp
	../../src/skins/meter/wg_piemeterskin.cpp
	../../src/skins/meter/wg_spinmeterskin.cpp
	../../src/skins/misc/wg_bakeskin.cpp
	../../src/skins/misc/wg_doubleskin.cpp
	../../src/skins/state/wg_blockskin.cpp
	../../src/skins/state/wg_boxskin.cpp
	../../src/skins/state/wg_colorskin.cpp
	../../src/skins/state/wg_lambdaskin.cpp
	../../src/skins/state/wg_multiblockskin.cpp
	../../src/skins/state/wg_scrollskin.cpp
	../../src/skins/state/wg_tileskin.cpp
	../../src/skins/static/wg_staticblockskin.cpp
	../../src/skins/static/wg_staticboxskin.cpp
	../../src/skins/static/wg_staticcolorskin.cpp
	../../src/skins/static/wg_staticgradientskin.cpp
	../../src/skins/static/wg_statictileskin.cpp

	../../src/textmappers/wg_textmapper.cpp
	../../src/textmappers/wg_stdtextmapper.cpp

	../../src/valueformatters/wg_standardformatter.cpp
	../../src/valueformatters/wg_timeformatter.cpp
	../../src/valueformatters/wg_valueformatter.cpp

	../../src/widgets/wg_animplayer.cpp
	../../src/widgets/wg_button.cpp
	../../src/widgets/wg_canvas.cpp
	../../src/widgets/wg_container.cpp
	../../src/widgets/wg_filler.cpp
	../../src/widgets/wg_fpsdisplay.cpp
	../../src/widgets/wg_image.cpp
	../../src/widgets/wg_knob.cpp
	../../src/widgets/wg_lineeditor.cpp
	../../src/widgets/wg_pianokeyboard.cpp
	../../src/widgets/wg_popupopener.cpp
	../../src/widgets/wg_rangeslider.cpp
	../../src/widgets/wg_rulerlabels.cpp
	../../src/widgets/wg_scrollbar.cpp
	../../src/widgets/wg_selectbox.cpp
	../../src/widgets/wg_sidecanvas.cpp
	../../src/widgets/wg_simplevolumemeter.cpp
	../../src/widgets/wg_slider.cpp
	../../src/widgets/wg_textdisplay.cpp
	../../src/widgets/wg_texteditor.cpp
	../../src/widgets/wg_timer.cpp
	../../src/widgets/wg_togglebutton.cpp
	../../src/widgets/wg_valuedisplay.cpp
	../../src/widgets/wg_volumemeter.cpp
	../../src/widgets/wg_widget.cpp

	../../src/widgets/capsules/wg_canvascapsule.cpp
	../../src/widgets/capsules/wg_capsule.cpp
	../../src/widgets/capsules/wg_renderlayercapsule.cpp
	../../src/widgets/capsules/wg_shadercapsule.cpp
	../../src/widgets/capsules/wg_sizecapsule.cpp
	../../src/widgets/layers/wg_designlayer.cpp
	../../src/widgets/layers/wg_dragndroplayer.cpp
	../../src/widgets/layers/wg_layer.cpp
	../../src/widgets/layers/wg_modallayer.cpp
	../../src/widgets/layers/wg_popuplayer.cpp
	../../src/widgets/layers/wg_tooltiplayer.cpp
	../../src/widgets/lists/wg_list.cpp
	../../src/widgets/lists/wg_packlist.cpp
	../../src/widgets/panels/wg_flexpanel.cpp
	../../src/widgets/panels/wg_lambdapanel.cpp
	../../src/widgets/panels/wg_packpanel.cpp
	../../src/widgets/panels/wg_panel.cpp
	../../src/widgets/panels/wg_rootpanel.cpp
	../../src/widgets/panels/wg_scrollpanel.cpp
	../../src/widgets/panels/wg_splitpanel.cpp
	../../src/widgets/panels/wg_stackpanel.cpp
)


################################
### OPENGL RENDERING LIBRARY ###
################################

ADD_LIBRARY( wgopenglgfx STATIC 

	../../src/gfxdevices/opengl/wg_glgfxdevice.cpp 
	../../src/gfxdevices/opengl/wg_glshaders.cpp 
	../../src/gfxdevices/opengl/wg_glsurface.cpp 
	../../src/gfxdevices/opengl/wg_glsurfacefactory.cpp
)

target_include_directories(wgopenglgfx PRIVATE 

	../../src/gfxdevices/opengl
)


##################################
### SOFTWARE RENDERING LIBRARY ###
##################################

ADD_LIBRARY( wgsoftgfx STATIC 

	../../src/gfxdevices/software/wg_softgfxdevice.cpp 
	../../src/gfxdevices/software/wg_softsurface.cpp 
	../../src/gfxdevices/software/wg_softsurfacefactory.cpp
)

target_include_directories(wgsoftgfx PRIVATE 

	../../src/gfxdevices/software
)

#############################
### FREETYPE FONT LIBRARY ###
#############################

ADD_LIBRARY( wgfreetypefont STATIC 

	../../src/fonts/freetype/wg_freetypefont.cpp
)

target_include_directories(wgfreetypefont PRIVATE 
	
	/usr/include/freetype2
	../../src/fonts/freetype
)

##############################
### EXAMPLE APPLICATION 01 ###
##############################

add_executable(example01

	../../src/examples/example01.cpp
)

target_link_libraries(example01 PUBLIC 
	wgsoftgfx
	wondergui 

	SDL2
)

target_include_directories(example01 PUBLIC	
	../../src/gfxdevices/software
)



#############################
### WORKBENCH APPLICATION ###
#############################

add_executable(testbench

	../../workbench/main2.cpp
)

target_link_libraries(testbench PUBLIC 
	wgopenglgfx
	wondergui 
	wgsoftgfx
	wgfreetypefont

	SDL2
	SDL2_image
	freetype
	GLEW
	GL
)

target_include_directories(testbench PUBLIC

	../../src/gfxdevices/software
	../../src/gfxdevices/opengl
	../../src/fonts/freetype
)

