source_group(Files REGULAR_EXPRESSION ".*")

source_group(Base REGULAR_EXPRESSION "base/*")
source_group(Components REGULAR_EXPRESSION "components/*")
source_group(SizeBrokers REGULAR_EXPRESSION "sizebrokers/*")

source_group(Skins REGULAR_EXPRESSION "skins/*")
source_group(Skins/Anim REGULAR_EXPRESSION "skins/anim/*")
source_group(Skins/Meter REGULAR_EXPRESSION "skins/meter/*")
source_group(Skins/Misc REGULAR_EXPRESSION "skins/misc/*")
source_group(Skins/State REGULAR_EXPRESSION "skins/state/*")
source_group(Skins/Static REGULAR_EXPRESSION "skins/static/*")

source_group(TextMappers REGULAR_EXPRESSION "textmappers/*")
source_group(ValueFormatters REGULAR_EXPRESSION "valueformatters/*")

source_group(Widgets REGULAR_EXPRESSION "widgets/*")
source_group(Widgets/Capsules REGULAR_EXPRESSION "widgets/capsules/*")
source_group(Widgets/Layers REGULAR_EXPRESSION "widgets/layers/*")
source_group(Widgets/Lists REGULAR_EXPRESSION "widgets/lists/*")
source_group(Widgets/Panels REGULAR_EXPRESSION "widgets/panels/*")


add_executable(spxworkbench

	main.cpp

	../../gfxdevices/software/wg_softgfxdevice.cpp
	../../gfxdevices/software/wg_softgfxdevice.h
	../../gfxdevices/software/wg_softsurface.cpp
	../../gfxdevices/software/wg_softsurface.h
	../../gfxdevices/software/wg_softsurfacefactory.cpp
	../../gfxdevices/software/wg_softsurfacefactory.h

    ../../baselibrary/base/wg_base.cpp
	../../baselibrary/base/wg_base.h
#	../../baselibrary/base/wg_bitmapfont.cpp
#	../../baselibrary/base/wg_bitmapfont.h
	../../baselibrary/base/wg_blob.cpp
	../../baselibrary/base/wg_blob.h
	../../baselibrary/base/wg_canvaslayers.cpp
	../../baselibrary/base/wg_canvaslayers.h
	../../baselibrary/base/wg_caret.cpp
	../../baselibrary/base/wg_caret.h
	../../baselibrary/base/wg_chain.cpp
	../../baselibrary/base/wg_chain.h
	../../baselibrary/base/wg_char.h
	../../baselibrary/base/wg_charbuffer.cpp
	../../baselibrary/base/wg_charbuffer.h
	../../baselibrary/base/wg_charseq.cpp
	../../baselibrary/base/wg_charseq.h
	../../baselibrary/base/wg_codepages.cpp
	../../baselibrary/base/wg_codepages.h
	../../baselibrary/base/wg_color.cpp
	../../baselibrary/base/wg_color.h
	../../baselibrary/base/wg_context.cpp
	../../baselibrary/base/wg_context.h
	../../baselibrary/base/wg_dataset.cpp
	../../baselibrary/base/wg_dataset.h
	../../baselibrary/base/wg_dummyfont.cpp
	../../baselibrary/base/wg_dummyfont.h
	../../baselibrary/base/wg_enumextras.cpp
	../../baselibrary/base/wg_enumextras.h
	../../baselibrary/base/wg_font.cpp
	../../baselibrary/base/wg_font.h
	../../baselibrary/base/wg_geo.cpp
	../../baselibrary/base/wg_geo.h
	../../baselibrary/base/wg_gfxdevice.cpp
	../../baselibrary/base/wg_gfxdevice.h
#	../../baselibrary/base/wg_gfxstream.h
#	../../baselibrary/base/wg_gfxstreamlogger.cpp
#	../../baselibrary/base/wg_gfxstreamlogger.h
#	../../baselibrary/base/wg_gfxstreamplayer.cpp
#	../../baselibrary/base/wg_gfxstreamplayer.h
#	../../baselibrary/base/wg_gfxstreamplug.cpp
#	../../baselibrary/base/wg_gfxstreamplug.h
#	../../baselibrary/base/wg_gfxstreamreader.cpp
#	../../baselibrary/base/wg_gfxstreamreader.h
#	../../baselibrary/base/wg_gfxstreamwriter.cpp
#	../../baselibrary/base/wg_gfxstreamwriter.h
	../../baselibrary/base/wg_gradient.cpp
	../../baselibrary/base/wg_gradient.h
	../../baselibrary/base/wg_hostbridge.h
	../../baselibrary/base/wg_inputhandler.cpp
	../../baselibrary/base/wg_inputhandler.h
	../../baselibrary/base/wg_internal.h
	../../baselibrary/base/wg_key.h
	../../baselibrary/base/wg_mempool.cpp
	../../baselibrary/base/wg_mempool.h
	../../baselibrary/base/wg_memstack.cpp
	../../baselibrary/base/wg_memstack.h
	../../baselibrary/base/wg_msg.cpp
	../../baselibrary/base/wg_msg.h
	../../baselibrary/base/wg_msglogger.cpp
	../../baselibrary/base/wg_msglogger.h
	../../baselibrary/base/wg_msgrouter.cpp
	../../baselibrary/base/wg_msgrouter.h
	../../baselibrary/base/wg_nullgfxdevice.cpp
	../../baselibrary/base/wg_nullgfxdevice.h
	../../baselibrary/base/wg_object.cpp
	../../baselibrary/base/wg_object.h
	../../baselibrary/base/wg_paddedslot.h
	../../baselibrary/base/wg_patches.cpp
	../../baselibrary/base/wg_patches.h
	../../baselibrary/base/wg_pointers.h
	../../baselibrary/base/wg_receiver.cpp
	../../baselibrary/base/wg_receiver.h
	../../baselibrary/base/wg_resdb.cpp
	../../baselibrary/base/wg_resdb.h
	../../baselibrary/base/wg_resloader.h
	../../baselibrary/base/wg_scrollbartarget.cpp
	../../baselibrary/base/wg_scrollbartarget.h
	../../baselibrary/base/wg_selectableslot.h
#	../../baselibrary/base/wg_shadow.h
	../../baselibrary/base/wg_skinslotmanager.cpp
	../../baselibrary/base/wg_skinslotmanager.h
	../../baselibrary/base/wg_slot.cpp
	../../baselibrary/base/wg_slot.h
	../../baselibrary/base/wg_slotholder.h
	../../baselibrary/base/wg_slotiterator.h
	../../baselibrary/base/wg_string.cpp
	../../baselibrary/base/wg_string.h
	../../baselibrary/base/wg_strongcomponentptr.h
	../../baselibrary/base/wg_strongptr.h
	../../baselibrary/base/wg_surface.cpp
	../../baselibrary/base/wg_surface.h
	../../baselibrary/base/wg_surfacefactory.cpp
	../../baselibrary/base/wg_surfacefactory.h
	../../baselibrary/base/wg_textlink.cpp
	../../baselibrary/base/wg_textlink.h
	../../baselibrary/base/wg_textstyle.cpp
	../../baselibrary/base/wg_textstyle.h
	../../baselibrary/base/wg_textstylemanager.cpp
	../../baselibrary/base/wg_textstylemanager.h
	../../baselibrary/base/wg_texttool.cpp
	../../baselibrary/base/wg_texttool.h
	../../baselibrary/base/wg_togglegroup.cpp
	../../baselibrary/base/wg_togglegroup.h
	../../baselibrary/base/wg_types.h
	../../baselibrary/base/wg_util.cpp
	../../baselibrary/base/wg_util.h
	../../baselibrary/base/wg_weakcomponentptr.h
	../../baselibrary/base/wg_weakptr.h


	../../baselibrary/components/wg_canimframes.cpp
	../../baselibrary/components/wg_canimframes.h
#	../../baselibrary/components/wg_ccanvas.cpp
#	../../baselibrary/components/wg_ccanvas.h
	../../baselibrary/components/wg_ccolumnheader.cpp
	../../baselibrary/components/wg_ccolumnheader.h
	../../baselibrary/components/wg_cdynamicslotcollection.cpp
	../../baselibrary/components/wg_cdynamicslotcollection.h
	../../baselibrary/components/wg_cdynamicslotvector.h
	../../baselibrary/components/wg_cdynamicslotvector.impl.h
	../../baselibrary/components/wg_cdynamicvector.h
	../../baselibrary/components/wg_cdynamicvector.impl.h
#	../../baselibrary/components/wg_cgfxinstream.cpp
#	../../baselibrary/components/wg_cgfxinstream.h
#	../../baselibrary/components/wg_cgfxoutstream.cpp
#	../../baselibrary/components/wg_cgfxoutstream.h
	../../baselibrary/components/wg_cicondisplay.cpp
	../../baselibrary/components/wg_cicondisplay.h
	../../baselibrary/components/wg_component.cpp
	../../baselibrary/components/wg_component.h
	../../baselibrary/components/wg_cpaddedslotvector.h
	../../baselibrary/components/wg_cpaddedslotvector.impl.h
	../../baselibrary/components/wg_cselectableslotvector.h
	../../baselibrary/components/wg_cselectableslotvector.impl.h
#	../../baselibrary/components/wg_cshadows.cpp
#	../../baselibrary/components/wg_cshadows.h
	../../baselibrary/components/wg_cskinslot.cpp
	../../baselibrary/components/wg_cskinslot.h
	../../baselibrary/components/wg_cskinslotmi.cpp
	../../baselibrary/components/wg_cskinslotmi.h
	../../baselibrary/components/wg_cslot.cpp
	../../baselibrary/components/wg_cslot.h
	../../baselibrary/components/wg_cslotarray.h
	../../baselibrary/components/wg_cstandardslot.cpp
	../../baselibrary/components/wg_cstandardslot.h
	../../baselibrary/components/wg_cstaticslotcollection.cpp
	../../baselibrary/components/wg_cstaticslotcollection.h
	../../baselibrary/components/wg_cstaticslotvector.h
	../../baselibrary/components/wg_cstaticslotvector.impl.h
	../../baselibrary/components/wg_cstatictextdisplay.cpp
	../../baselibrary/components/wg_cstatictextdisplay.h
	../../baselibrary/components/wg_cstaticvaluedisplay.cpp
	../../baselibrary/components/wg_cstaticvaluedisplay.h
	../../baselibrary/components/wg_cstaticvector.h
	../../baselibrary/components/wg_cstaticvector.impl.h
	../../baselibrary/components/wg_ctext.cpp
	../../baselibrary/components/wg_ctext.h
	../../baselibrary/components/wg_ctextdisplay.cpp
	../../baselibrary/components/wg_ctextdisplay.h
	../../baselibrary/components/wg_ctexteditor.cpp
	../../baselibrary/components/wg_ctexteditor.h
	../../baselibrary/components/wg_cvaluedisplay.cpp
	../../baselibrary/components/wg_cvaluedisplay.h
	../../baselibrary/components/wg_cvalueeditor.cpp
	../../baselibrary/components/wg_cvalueeditor.h
	../../baselibrary/components/wg_geocomponent.cpp
	../../baselibrary/components/wg_geocomponent.h

	../../baselibrary/sizebrokers/wg_scalepreferred_sizebroker.cpp
	../../baselibrary/sizebrokers/wg_scalepreferred_sizebroker.h
	../../baselibrary/sizebrokers/wg_sizebroker.cpp
	../../baselibrary/sizebrokers/wg_sizebroker.h
	../../baselibrary/sizebrokers/wg_uniform_sizebroker.cpp
	../../baselibrary/sizebrokers/wg_uniform_sizebroker.h
	
	../../baselibrary/skins/wg_animskin.cpp
	../../baselibrary/skins/wg_animskin.h
	../../baselibrary/skins/wg_skin.cpp
	../../baselibrary/skins/wg_skin.h
	../../baselibrary/skins/wg_skin.impl.h
	../../baselibrary/skins/wg_stateskin.cpp
	../../baselibrary/skins/wg_stateskin.h	
	../../baselibrary/skins/anim/wg_spinanimskin.cpp
	../../baselibrary/skins/anim/wg_spinanimskin.h
	../../baselibrary/skins/meter/wg_fillmeterskin.cpp
	../../baselibrary/skins/meter/wg_fillmeterskin.h
	../../baselibrary/skins/meter/wg_framemeterskin.cpp
	../../baselibrary/skins/meter/wg_framemeterskin.h
	../../baselibrary/skins/meter/wg_piemeterskin.cpp
	../../baselibrary/skins/meter/wg_piemeterskin.h
	../../baselibrary/skins/meter/wg_spinmeterskin.cpp
  	../../baselibrary/skins/meter/wg_spinmeterskin.h
	../../baselibrary/skins/misc/wg_bakeskin.cpp
	../../baselibrary/skins/misc/wg_bakeskin.h
	../../baselibrary/skins/misc/wg_doubleskin.cpp
	../../baselibrary/skins/misc/wg_doubleskin.h
	../../baselibrary/skins/state/wg_blockskin.cpp
	../../baselibrary/skins/state/wg_blockskin.h
	../../baselibrary/skins/state/wg_boxskin.cpp
	../../baselibrary/skins/state/wg_boxskin.h
	../../baselibrary/skins/state/wg_circleskin.cpp
	../../baselibrary/skins/state/wg_circleskin.h
	../../baselibrary/skins/state/wg_colorskin.cpp
	../../baselibrary/skins/state/wg_colorskin.h
#	../../baselibrary/skins/state/wg_lambdaskin.cpp
#	../../baselibrary/skins/state/wg_lambdaskin.h
	../../baselibrary/skins/state/wg_multiblockskin.cpp
	../../baselibrary/skins/state/wg_multiblockskin.h
	../../baselibrary/skins/state/wg_scrollskin.cpp
	../../baselibrary/skins/state/wg_scrollskin.h
	../../baselibrary/skins/state/wg_tileskin.cpp
	../../baselibrary/skins/state/wg_tileskin.h
	../../baselibrary/skins/static/wg_staticblockskin.cpp
	../../baselibrary/skins/static/wg_staticblockskin.h
	../../baselibrary/skins/static/wg_staticboxskin.cpp
	../../baselibrary/skins/static/wg_staticboxskin.h
	../../baselibrary/skins/static/wg_staticcolorskin.cpp
	../../baselibrary/skins/static/wg_staticcolorskin.h
	../../baselibrary/skins/static/wg_staticgradientskin.cpp
	../../baselibrary/skins/static/wg_staticgradientskin.h
	../../baselibrary/skins/static/wg_statictileskin.cpp
	../../baselibrary/skins/static/wg_statictileskin.h

	../../baselibrary/textmappers/wg_stdtextmapper.cpp
	../../baselibrary/textmappers/wg_stdtextmapper.h
	../../baselibrary/textmappers/wg_textmapper.cpp
	../../baselibrary/textmappers/wg_textmapper.h

	../../baselibrary/valueformatters/wg_standardformatter.cpp
	../../baselibrary/valueformatters/wg_standardformatter.h
	../../baselibrary/valueformatters/wg_timeformatter.cpp
	../../baselibrary/valueformatters/wg_timeformatter.h
	../../baselibrary/valueformatters/wg_valueformatter.cpp
	../../baselibrary/valueformatters/wg_valueformatter.h

	../../baselibrary/widgets/wg_animplayer.cpp
	../../baselibrary/widgets/wg_animplayer.h
	../../baselibrary/widgets/wg_button.cpp
	../../baselibrary/widgets/wg_button.h
#	../../baselibrary/widgets/wg_canvas.cpp
#	../../baselibrary/widgets/wg_canvas.h
	../../baselibrary/widgets/wg_container.cpp
	../../baselibrary/widgets/wg_container.h
	../../baselibrary/widgets/wg_filler.cpp
	../../baselibrary/widgets/wg_filler.h
	../../baselibrary/widgets/wg_fpsdisplay.cpp
	../../baselibrary/widgets/wg_fpsdisplay.h
	../../baselibrary/widgets/wg_image.cpp
	../../baselibrary/widgets/wg_image.h
	../../baselibrary/widgets/wg_knob.cpp
	../../baselibrary/widgets/wg_knob.h
	../../baselibrary/widgets/wg_lineeditor.cpp
	../../baselibrary/widgets/wg_lineeditor.h
	../../baselibrary/widgets/wg_pianokeyboard.cpp
	../../baselibrary/widgets/wg_pianokeyboard.h
	../../baselibrary/widgets/wg_popupopener.cpp
	../../baselibrary/widgets/wg_popupopener.h
	../../baselibrary/widgets/wg_rangeslider.cpp
	../../baselibrary/widgets/wg_rangeslider.h
#	../../baselibrary/widgets/wg_rulerlabels.cpp
#	../../baselibrary/widgets/wg_rulerlabels.h
	../../baselibrary/widgets/wg_scrollbar.cpp
	../../baselibrary/widgets/wg_scrollbar.h
	../../baselibrary/widgets/wg_selectbox.cpp
	../../baselibrary/widgets/wg_selectbox.h
	../../baselibrary/widgets/wg_sidecanvas.cpp
	../../baselibrary/widgets/wg_sidecanvas.h
#	../../baselibrary/widgets/wg_simplevolumemeter.cpp
#	../../baselibrary/widgets/wg_simplevolumemeter.h
	../../baselibrary/widgets/wg_slider.cpp
	../../baselibrary/widgets/wg_slider.h
	../../baselibrary/widgets/wg_textdisplay.cpp
	../../baselibrary/widgets/wg_textdisplay.h
	../../baselibrary/widgets/wg_texteditor.cpp
	../../baselibrary/widgets/wg_texteditor.h
	../../baselibrary/widgets/wg_timer.cpp
	../../baselibrary/widgets/wg_timer.h
	../../baselibrary/widgets/wg_togglebutton.cpp
	../../baselibrary/widgets/wg_togglebutton.h
	../../baselibrary/widgets/wg_valuedisplay.cpp
	../../baselibrary/widgets/wg_valuedisplay.h
#	../../baselibrary/widgets/wg_volumemeter.cpp
#	../../baselibrary/widgets/wg_volumemeter.h
	../../baselibrary/widgets/wg_widget.cpp
	../../baselibrary/widgets/wg_widget.h

	../../baselibrary/widgets/capsules/wg_canvascapsule.cpp
	../../baselibrary/widgets/capsules/wg_canvascapsule.h
	../../baselibrary/widgets/capsules/wg_capsule.cpp
	../../baselibrary/widgets/capsules/wg_capsule.h
	../../baselibrary/widgets/capsules/wg_renderlayercapsule.cpp
	../../baselibrary/widgets/capsules/wg_renderlayercapsule.h
	../../baselibrary/widgets/capsules/wg_shadercapsule.cpp
	../../baselibrary/widgets/capsules/wg_shadercapsule.h
	../../baselibrary/widgets/capsules/wg_sizecapsule.cpp
	../../baselibrary/widgets/capsules/wg_sizecapsule.h
	../../baselibrary/widgets/layers/wg_designlayer.cpp
	../../baselibrary/widgets/layers/wg_designlayer.h
	../../baselibrary/widgets/layers/wg_dragndroplayer.cpp
	../../baselibrary/widgets/layers/wg_dragndroplayer.h
	../../baselibrary/widgets/layers/wg_layer.cpp
	../../baselibrary/widgets/layers/wg_layer.h
	../../baselibrary/widgets/layers/wg_modallayer.cpp
	../../baselibrary/widgets/layers/wg_modallayer.h
	../../baselibrary/widgets/layers/wg_popuplayer.cpp
	../../baselibrary/widgets/layers/wg_popuplayer.h
	../../baselibrary/widgets/layers/wg_tooltiplayer.cpp
	../../baselibrary/widgets/layers/wg_tooltiplayer.h
	../../baselibrary/widgets/lists/wg_list.cpp
	../../baselibrary/widgets/lists/wg_list.h
	../../baselibrary/widgets/lists/wg_packlist.cpp
	../../baselibrary/widgets/lists/wg_packlist.h
	../../baselibrary/widgets/panels/wg_flexpanel.cpp
	../../baselibrary/widgets/panels/wg_flexpanel.h
	../../baselibrary/widgets/panels/wg_lambdapanel.cpp
	../../baselibrary/widgets/panels/wg_lambdapanel.h
	../../baselibrary/widgets/panels/wg_packpanel.cpp
	../../baselibrary/widgets/panels/wg_packpanel.h
	../../baselibrary/widgets/panels/wg_panel.cpp
	../../baselibrary/widgets/panels/wg_panel.h
	../../baselibrary/widgets/panels/wg_rootpanel.cpp 
	../../baselibrary/widgets/panels/wg_rootpanel.h
#	../../baselibrary/widgets/panels/wg_scrollpanel.cpp
#	../../baselibrary/widgets/panels/wg_scrollpanel.h
	../../baselibrary/widgets/panels/wg_splitpanel.cpp
	../../baselibrary/widgets/panels/wg_splitpanel.h
	../../baselibrary/widgets/panels/wg_stackpanel.cpp
	../../baselibrary/widgets/panels/wg_stackpanel.h

)

target_link_directories(spxworkbench PRIVATE 
)

target_working_directory(spxworkbench ${WG_ROOT})

target_link_libraries(spxworkbench PRIVATE
#	wgsoftgfx
#	wgopenglgfx
	wgfreetypefont
#	wondergui 
	${SDL2_LIBS}
	${SDL2IMAGE_LIBS}
	${OPENGL_LIBS}
)
