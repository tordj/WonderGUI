
source_group(Base REGULAR_EXPRESSION "base//*")
source_group(C-Interface REGULAR_EXPRESSION "c-interface//*")
source_group(Components REGULAR_EXPRESSION "components//*")

source_group(Skins REGULAR_EXPRESSION "skins//*")
source_group(Skins/Anim REGULAR_EXPRESSION "skins/anim//*")
source_group(Skins/Meter REGULAR_EXPRESSION "skins/meter//*")
source_group(Skins/Misc REGULAR_EXPRESSION "skins/misc//*")
source_group(Skins/State REGULAR_EXPRESSION "skins/state//*")
source_group(Skins/Static REGULAR_EXPRESSION "skins/static//*")

source_group(Slots REGULAR_EXPRESSION "slots//*")

source_group(SurfaceIO REGULAR_EXPRESSION "surfaceio//*")

source_group(GfxStream REGULAR_EXPRESSION "gfxstream//*")
source_group(TextLayouts REGULAR_EXPRESSION "textlayouts//*")
source_group(NumberLayouts REGULAR_EXPRESSION "numberlayouts//*")

source_group(Widgets REGULAR_EXPRESSION "widgets//*")
source_group(Widgets/Capsules REGULAR_EXPRESSION "widgets/capsules//*")
source_group(Widgets/Overlays REGULAR_EXPRESSION "widgets/overlays//*")
source_group(Widgets/Lists REGULAR_EXPRESSION "widgets/lists//*")
source_group(Widgets/Panels REGULAR_EXPRESSION "widgets/panels//*")


add_library( wondergui STATIC
			
	base/wg_animframes.cpp
	base/wg_animframes.h
	base/wg_base.cpp
	base/wg_base.h
	base/wg_bitmapcache.cpp
	base/wg_bitmapcache.h
	base/wg_bitmapfont.cpp
	base/wg_bitmapfont.h
	base/wg_blob.cpp
	base/wg_blob.h
	base/wg_canvaslayers.cpp
	base/wg_canvaslayers.h
	base/wg_caret.cpp
	base/wg_caret.h
	base/wg_chain.cpp
	base/wg_chain.h
	base/wg_char.h
	base/wg_charbuffer.cpp
	base/wg_charbuffer.h
	base/wg_charseq.cpp
	base/wg_charseq.h
	base/wg_codepages.cpp
	base/wg_codepages.h
	base/wg_color.cpp
	base/wg_color.h
	base/wg_componentptr.h
	base/wg_context.cpp
	base/wg_context.h
	base/wg_dataset.cpp
	base/wg_dataset.h
	base/wg_dummyfont.cpp
	base/wg_dummyfont.h
	base/wg_dynamicvector.h
	base/wg_enumextras.cpp
	base/wg_enumextras.h
	base/wg_font.cpp
	base/wg_font.h
	base/wg_geo.cpp
	base/wg_geo.h
	base/wg_gfxdevice.cpp
	base/wg_gfxdevice.h
	base/wg_gridwalker.cpp
	base/wg_gridwalker.h
	base/wg_gradient.cpp
	base/wg_gradient.h
	base/wg_hostbridge.h
	base/wg_inputhandler.cpp
	base/wg_inputhandler.h
	base/wg_key.h
	base/wg_memheap.cpp
	base/wg_memheap.h
	base/wg_mempool.cpp
	base/wg_mempool.h
	base/wg_memstack.cpp
	base/wg_memstack.h
	base/wg_msg.cpp
	base/wg_msg.h
	base/wg_msglogger.cpp
	base/wg_msglogger.h
	base/wg_msgrouter.cpp
	base/wg_msgrouter.h
	base/wg_nullgfxdevice.cpp
	base/wg_nullgfxdevice.h
	base/wg_object.cpp
	base/wg_object.h
	base/wg_packlayout.cpp
	base/wg_packlayout.h
	base/wg_patches.cpp
	base/wg_patches.impl.h
	base/wg_patches.h
	base/wg_pixeltools.cpp
	base/wg_pixeltools.h
	base/wg_pointers.h
	base/wg_printer.cpp
	base/wg_printer.h
	base/wg_receiver.cpp
	base/wg_receiver.h
	base/wg_resdb.cpp
	base/wg_resdb.h
	base/wg_resloader.h
	base/wg_root.cpp
	base/wg_root.h
	base/wg_skinslot.cpp
	base/wg_skinslot.h
	base/wg_skinslotmanager.cpp
	base/wg_skinslotmanager.h
	base/wg_skinslotmi.cpp
	base/wg_skinslotmi.h
	base/wg_state.cpp
	base/wg_state.h
	base/wg_staticvector.h
	base/wg_string.cpp
	base/wg_string.h
	base/wg_strongptr.h
	base/wg_surface.cpp
	base/wg_surface.h
	base/wg_surfacefactory.cpp
	base/wg_surfacefactory.h
	base/wg_textlink.cpp
	base/wg_textlink.h
	base/wg_textstyle.cpp
	base/wg_textstyle.h
	base/wg_textstylemanager.cpp
	base/wg_textstylemanager.h
	base/wg_texttool.cpp
	base/wg_texttool.h
	base/wg_togglegroup.cpp
	base/wg_togglegroup.h
	base/wg_types.h
	base/wg_util.cpp
	base/wg_util.h
	base/wg_weakptr.h


	c-interface/wg_c_base.cpp
	c-interface/wg_c_base.h
	c-interface/wg_c_blob.cpp
	c-interface/wg_c_blob.h
	c-interface/wg_c_bitmapcache.cpp
	c-interface/wg_c_bitmapcache.h
	c-interface/wg_c_bitmapfont.cpp
	c-interface/wg_c_bitmapfont.h
	c-interface/wg_c_callcollection.cpp
	c-interface/wg_c_callcollection.h
	c-interface/wg_c_canvaslayers.cpp
	c-interface/wg_c_canvaslayers.h
	c-interface/wg_c_color.cpp
	c-interface/wg_c_color.h
	c-interface/wg_c_font.cpp
	c-interface/wg_c_font.h		
	c-interface/wg_c_geo.cpp
	c-interface/wg_c_geo.h
	c-interface/wg_c_gfxstreambuffer.cpp
	c-interface/wg_c_gfxstreambuffer.h
	c-interface/wg_c_gfxstreamlogger.cpp
	c-interface/wg_c_gfxstreamlogger.h
	c-interface/wg_c_gfxstreamloopwrapper.cpp
	c-interface/wg_c_gfxstreamloopwrapper.h
	c-interface/wg_c_gfxstreamplayer.cpp
	c-interface/wg_c_gfxstreamplayer.h
	c-interface/wg_c_gfxstreampump.cpp
	c-interface/wg_c_gfxstreampump.h
	c-interface/wg_c_gfxstreamreader.cpp
	c-interface/wg_c_gfxstreamreader.h
	c-interface/wg_c_gfxstreamsplitter.cpp
	c-interface/wg_c_gfxstreamsplitter.h
	c-interface/wg_c_gfxstreamwrapper.cpp
	c-interface/wg_c_gfxstreamwrapper.h
	c-interface/wg_c_gradient.cpp
	c-interface/wg_c_gradient.h
	c-interface/wg_c_gfxdevice.cpp
	c-interface/wg_c_gfxdevice.h
	c-interface/wg_c_internal.cpp
	c-interface/wg_c_internal.h
	c-interface/wg_c_memheap.cpp
	c-interface/wg_c_memheap.h
	c-interface/wg_c_object.cpp
	c-interface/wg_c_object.h
	c-interface/wg_c_printer.cpp
	c-interface/wg_c_printer.h
	c-interface/wg_c_surface.cpp
	c-interface/wg_c_surface.h
	c-interface/wg_c_surfacefactory.cpp
	c-interface/wg_c_surfacefactory.h
	c-interface/wg_c_surfacereader.cpp
	c-interface/wg_c_surfacereader.h
	c-interface/wg_c_surfacewriter.cpp
	c-interface/wg_c_surfacewriter.h
	c-interface/wg_c_types.h
	c-interface/wg_c_types.cpp


	components/wg_columnheader.cpp
	components/wg_columnheader.h
	components/wg_editablenumber.cpp
	components/wg_editablenumber.h
	components/wg_editabletext.cpp
	components/wg_editabletext.h
	components/wg_icon.cpp
	components/wg_icon.h
	components/wg_number.cpp
	components/wg_number.h
	components/wg_scroller.cpp
	components/wg_scroller.h
	components/wg_staticnumber.cpp
	components/wg_staticnumber.h
	components/wg_statictext.cpp
	components/wg_statictext.h
	components/wg_text.cpp
	components/wg_text.h
	components/wg_textbase.cpp
	components/wg_textbase.h
	components/wg_component.cpp
	components/wg_component.h

	gfxstream/wg_gfxstream.h
	gfxstream/wg_gfxstreambuffer.cpp
	gfxstream/wg_gfxstreambuffer.h
	gfxstream/wg_gfxstreamdecoder.cpp
	gfxstream/wg_gfxstreamdecoder.h
	gfxstream/wg_gfxstreamencoder.cpp
	gfxstream/wg_gfxstreamencoder.h
	gfxstream/wg_gfxstreamlogger.cpp
	gfxstream/wg_gfxstreamlogger.h
	gfxstream/wg_gfxstreamloopwrapper.cpp
	gfxstream/wg_gfxstreamloopwrapper.h
	gfxstream/wg_gfxstreamplayer.cpp
	gfxstream/wg_gfxstreamplayer.h
	gfxstream/wg_gfxstreampump.cpp
	gfxstream/wg_gfxstreampump.h
	gfxstream/wg_gfxstreamreader.cpp
	gfxstream/wg_gfxstreamreader.h
	gfxstream/wg_gfxstreamsink.h
	gfxstream/wg_gfxstreamsource.h
	gfxstream/wg_gfxstreamsplitter.cpp
	gfxstream/wg_gfxstreamsplitter.h
	gfxstream/wg_gfxstreamwrapper.cpp
	gfxstream/wg_gfxstreamwrapper.h
	gfxstream/wg_gfxstreamwriter.cpp
	gfxstream/wg_gfxstreamwriter.h

	numberlayouts/wg_basicnumberlayout.cpp
	numberlayouts/wg_basicnumberlayout.h
	numberlayouts/wg_numberlayout.cpp
	numberlayouts/wg_numberlayout.h

	skins/wg_animskin.cpp
	skins/wg_animskin.h
	skins/wg_skin.cpp
	skins/wg_skin.h
	skins/wg_skin.impl.h
	skins/wg_stateskin.cpp
	skins/wg_stateskin.h	
	skins/anim/wg_spinanimskin.cpp
	skins/anim/wg_spinanimskin.h
	skins/meter/wg_fillmeterskin.cpp
	skins/meter/wg_fillmeterskin.h
	skins/meter/wg_framemeterskin.cpp
	skins/meter/wg_framemeterskin.h
	skins/meter/wg_piemeterskin.cpp
	skins/meter/wg_piemeterskin.h
	skins/meter/wg_spinmeterskin.cpp
	skins/meter/wg_spinmeterskin.h
	skins/misc/wg_bakeskin.cpp
	skins/misc/wg_bakeskin.h
	skins/misc/wg_doubleskin.cpp
	skins/misc/wg_doubleskin.h
	skins/state/wg_blockskin.cpp
	skins/state/wg_blockskin.h
	skins/state/wg_boxskin.cpp
	skins/state/wg_boxskin.h
	skins/state/wg_circleskin.cpp
	skins/state/wg_circleskin.h
	skins/state/wg_colorskin.cpp
	skins/state/wg_colorskin.h
	skins/state/wg_multiblockskin.cpp
	skins/state/wg_multiblockskin.h
	skins/state/wg_scrollskin.cpp
	skins/state/wg_scrollskin.h
	skins/state/wg_tileskin.cpp
	skins/state/wg_tileskin.h
	skins/static/wg_staticblockskin.cpp
	skins/static/wg_staticblockskin.h
	skins/static/wg_staticboxskin.cpp
	skins/static/wg_staticboxskin.h
	skins/static/wg_staticcolorskin.cpp
	skins/static/wg_staticcolorskin.h
	skins/static/wg_staticgradientskin.cpp
	skins/static/wg_staticgradientskin.h
	skins/static/wg_statictileskin.cpp
	skins/static/wg_statictileskin.h

	slots/wg_slot.cpp
	slots/wg_slot.h
	slots/wg_slotextras.impl.h
	slots/wg_slotextras.h
	slots/wg_slotholder.h
	slots/wg_slotiterator.h
	slots/wg_dynamicslotcollection.h
	slots/wg_dynamicslotvector.h
	slots/wg_dynamicslotvector.impl.h
	slots/wg_slotarray.h
	slots/wg_slotarray2.h
	slots/wg_staticslotcollection.h
	slots/wg_staticslotvector.h
	slots/wg_staticslotvector.impl.h

	surfaceio/wg_surfacefileheader.h
	surfaceio/wg_surfacefileinspector.h
	surfaceio/wg_surfacefileinspector.cpp
	surfaceio/wg_surfacereader.h
	surfaceio/wg_surfacereader.cpp
	surfaceio/wg_surfacewriter.h
	surfaceio/wg_surfacewriter.cpp




	textlayouts/wg_basictextlayout.cpp
	textlayouts/wg_basictextlayout.h
	textlayouts/wg_textlayout.cpp
	textlayouts/wg_textlayout.h

	widgets/wg_animplayer.cpp
	widgets/wg_animplayer.h
	widgets/wg_button.cpp
	widgets/wg_button.h
	widgets/wg_container.cpp
	widgets/wg_container.h
	widgets/wg_filler.cpp
	widgets/wg_filler.h
	widgets/wg_fpsdisplay.cpp
	widgets/wg_fpsdisplay.h
	widgets/wg_image.cpp
	widgets/wg_image.h
	widgets/wg_knob.cpp
	widgets/wg_knob.h
	widgets/wg_lineeditor.cpp
	widgets/wg_lineeditor.h
	widgets/wg_pianokeyboard.cpp
	widgets/wg_pianokeyboard.h
	widgets/wg_popupopener.cpp
	widgets/wg_popupopener.h
	widgets/wg_rangeslider.cpp
	widgets/wg_rangeslider.h
	widgets/wg_scrollbar.cpp
	widgets/wg_scrollbar.h
	widgets/wg_selectbox.cpp
	widgets/wg_selectbox.h
	widgets/wg_sidecanvas.cpp
	widgets/wg_sidecanvas.h
	widgets/wg_slider.cpp
	widgets/wg_slider.h
	widgets/wg_surfacedisplay.cpp
	widgets/wg_surfacedisplay.h
	widgets/wg_textdisplay.cpp
	widgets/wg_textdisplay.h
	widgets/wg_texteditor.cpp
	widgets/wg_texteditor.h
	widgets/wg_timer.cpp
	widgets/wg_timer.h
	widgets/wg_togglebutton.cpp
	widgets/wg_togglebutton.h
	widgets/wg_numberdisplay.cpp
	widgets/wg_numberdisplay.h
	widgets/wg_widget.cpp
	widgets/wg_widget.h

	widgets/capsules/wg_canvascapsule.cpp
	widgets/capsules/wg_canvascapsule.h
	widgets/capsules/wg_capsule.cpp
	widgets/capsules/wg_capsule.h
	widgets/capsules/wg_renderlayercapsule.cpp
	widgets/capsules/wg_renderlayercapsule.h
	widgets/capsules/wg_sizecapsule.cpp
	widgets/capsules/wg_sizecapsule.h
	widgets/overlays/wg_designoverlay.cpp
	widgets/overlays/wg_designoverlay.h
	widgets/overlays/wg_dragndropoverlay.cpp
	widgets/overlays/wg_dragndropoverlay.h
	widgets/overlays/wg_overlay.cpp
	widgets/overlays/wg_overlay.h
	widgets/overlays/wg_modaloverlay.cpp
	widgets/overlays/wg_modaloverlay.h
	widgets/overlays/wg_popupoverlay.cpp
	widgets/overlays/wg_popupoverlay.h
	widgets/overlays/wg_tooltipoverlay.cpp
	widgets/overlays/wg_tooltipoverlay.h
	widgets/lists/wg_list.cpp
	widgets/lists/wg_list.h
	widgets/lists/wg_packlist.cpp
	widgets/lists/wg_packlist.h
	widgets/panels/wg_flexpanel.cpp
	widgets/panels/wg_flexpanel.h
	widgets/panels/wg_lambdapanel.cpp
	widgets/panels/wg_lambdapanel.h
	widgets/panels/wg_packpanel.cpp
	widgets/panels/wg_packpanel.h
	widgets/panels/wg_panel.cpp
	widgets/panels/wg_panel.h
	widgets/panels/wg_rootpanel.cpp
	widgets/panels/wg_rootpanel.h
	widgets/panels/wg_scrollpanel.cpp
	widgets/panels/wg_scrollpanel.h
	widgets/panels/wg_splitpanel.cpp
	widgets/panels/wg_splitpanel.h
	widgets/panels/wg_stackpanel.cpp
	widgets/panels/wg_stackpanel.h
	widgets/panels/wg_twoslotpanel.cpp
	widgets/panels/wg_twoslotpanel.h
)
