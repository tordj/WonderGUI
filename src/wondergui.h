/*=========================================================================

						 >>> WonderGUI <<<

  This file is part of Tord Jansson's WonderGUI Graphics Toolkit
  and copyright (c) Tord Jansson, Sweden [tord.jansson@gmail.com].

							-----------

  The WonderGUI Graphics Toolkit is free software; you can redistribute
  this file and/or modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

							-----------

  The WonderGUI Graphics Toolkit is also available for use in commercial
  closed-source projects under a separate license. Interested parties
  should contact Tord Jansson [tord.jansson@gmail.com] for details.

=========================================================================*/

#ifndef WONDERGUI_DOT_H
#define WONDERGUI_DOT_H
#pragma once

#include <wg_anim.h>
#include <wg_base.h>
#include <wg_bitmapfont.h>
#include <wg_blob.h>
#include <wg_caret.h>
#include <wg_chain.h>
#include <wg_char.h>
#include <wg_charbuffer.h>
#include <wg_charseq.h>
#include <wg_childiterator.h>
#include <wg_codepages.h>
#include <wg_color.h>
#include <wg_componentholder.h>
#include <wg_context.h>
#include <wg_dummyfont.h>
#include <wg_enumextras.h>
#include <wg_font.h>
#include <wg_geo.h>
#include <wg_gfxanim.h>
#include <wg_gfxdevice.h>
#include <wg_gfxinstream.h>
#include <wg_gfxoutstream.h>
#include <wg_gfxstream.h>
#include <wg_gfxstreamlogger.h>
#include <wg_gfxstreamplayer.h>
#include <wg_gfxstreamplug.h>
#include <wg_gfxstreamreader.h>
#include <wg_gfxstreamwriter.h>
#include <wg_inputhandler.h>
#include <wg_key.h>
#include <wg_mempool.h>
#include <wg_memstack.h>
#include <wg_msg.h>
#include <wg_msglogger.h>
#include <wg_msgrouter.h>
#include <wg_nullgfxdevice.h>
#include <wg_object.h>
#include <wg_paddedslot.h>
#include <wg_patches.h>
#include <wg_payload.h>
#include <wg_pointers.h>
#include <wg_qpix.h>
#include <wg_receiver.h>
#include <wg_resdb.h>
#include <wg_resloader.h>
#include <wg_scrollbartarget.h>
#include <wg_shadow.h>
#include <wg_slot.h>
#include <wg_slotarray.h>
#include <wg_string.h>
#include <wg_stronginterfaceptr.h>
#include <wg_strongptr.h>
#include <wg_surface.h>
#include <wg_surfacefactory.h>
#include <wg_textlink.h>
#include <wg_textstyle.h>
#include <wg_textstylemanager.h>
#include <wg_texttool.h>
#include <wg_togglegroup.h>
#include <wg_types.h>
#include <wg_util.h>
#include <wg_weakinterfaceptr.h>
#include <wg_weakptr.h>
#include <wg_widgetholder.h>
#include <wg_ccanvas.h>
#include <wg_ccolumnheader.h>
#include <wg_cicondisplay.h>
#include <wg_component.h>
#include <wg_cspan.h>
#include <wg_ctext.h>
#include <wg_ctextdisplay.h>
#include <wg_ctexteditor.h>
#include <wg_cvalue.h>
#include <wg_cvaluedisplay.h>
#include <wg_cvalueeditor.h>
#include <wg_icanvas.h>
#include <wg_ichildren.h>
#include <wg_icolumnheader.h>
#include <wg_idynamicchildren.h>
#include <wg_idynamicslots.h>
#include <wg_ihideablechildren.h>
#include <wg_iicondisplay.h>
#include <wg_imodspan.h>
#include <wg_interface.h>
#include <wg_ipaddedchildren.h>
#include <wg_ipaddedslotarray.h>
#include <wg_irotextdisplay.h>
#include <wg_irovaluedisplay.h>
#include <wg_iselectablechildren.h>
#include <wg_iselectableslotarray.h>
#include <wg_ishadows.h>
#include <wg_islidespan.h>
#include <wg_islot.h>
#include <wg_islotarray.h>
#include <wg_islots.h>
#include <wg_ispan.h>
#include <wg_itextdisplay.h>
#include <wg_itexteditor.h>
#include <wg_ivaluedisplay.h>
#include <wg_ivalueeditor.h>
#include <wg_iweightedchildren.h>
#include <wg_scalepreferred_sizebroker.h>
#include <wg_sizebroker.h>
#include <wg_uniform_sizebroker.h>
#include <wg_blockskin.h>
#include <wg_boxskin.h>
#include <wg_colorskin.h>
#include <wg_extendedskin.h>
#include <wg_multiblockskin.h>
#include <wg_skin.h>
#include <wg_staticcolorskin.h>
#include <wg_stdtextmapper.h>
#include <wg_textmapper.h>
#include <wg_standardformatter.h>
#include <wg_timeformatter.h>
#include <wg_valueformatter.h>
#include <wg_capsule.h>
#include <wg_shadercapsule.h>
#include <wg_sizecapsule.h>
#include <wg_dragndroplayer.h>
#include <wg_layer.h>
#include <wg_modallayer.h>
#include <wg_popuplayer.h>
#include <wg_list.h>
#include <wg_packlist.h>
#include <wg_flexpanel.h>
#include <wg_lambdapanel.h>
#include <wg_packpanel.h>
#include <wg_panel.h>
#include <wg_rootpanel.h>
#include <wg_scrollpanel.h>
#include <wg_splitpanel.h>
#include <wg_stackpanel.h>
#include <wg_animplayer.h>
#include <wg_button.h>
#include <wg_canvas.h>
#include <wg_container.h>
#include <wg_filler.h>
#include <wg_fpsdisplay.h>
#include <wg_image.h>
#include <wg_lineeditor.h>
#include <wg_popupopener.h>
#include <wg_refreshbutton.h>
#include <wg_rulerlabels.h>
#include <wg_scrollbar.h>
#include <wg_simplevolumemeter.h>
#include <wg_textdisplay.h>
#include <wg_texteditor.h>
#include <wg_togglebutton.h>
#include <wg_valuedisplay.h>
#include <wg_volumemeter.h>
#include <wg_widget.h>

#endif //WONDERGUI_DOT_H
