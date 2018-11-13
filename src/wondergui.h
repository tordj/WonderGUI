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
#include <wg_dummyfont.h>
#include <wg_font.h>
#include <wg_geo.h>
#include <wg_gfxanim.h>
#include <wg_gfxdevice.h>
#include <wg_gfxinstream.h>
#include <wg_gfxoutstream.h>
#include <wg_gfxstreamreader.h>
#include <wg_gfxstreamwriter.h>
#include <wg_gfxstreamplug.h>
#include <wg_gfxstreamlogger.h>
#include <wg_gfxstreamplayer.h>
#include <wg_inputhandler.h>
#include <wg_itemholder.h>
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
#include <wg_pointers.h>
#include <wg_receiver.h>
#include <wg_resdb.h>
#include <wg_resloader.h>
#include <wg_scrollbartarget.h>
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
#include <wg_child.h>
#include <wg_children.h>
#include <wg_columnheader.h>
#include <wg_dynamicchildren.h>
#include <wg_edittext.h>
#include <wg_editvalue.h>
#include <wg_hideablechildren.h>
#include <wg_icon.h>
#include <wg_interface.h>
#include <wg_modspan.h>
#include <wg_modtext.h>
#include <wg_modvalue.h>
#include <wg_paddedchildren.h>
#include <wg_selectablechildren.h>
#include <wg_slidespan.h>
#include <wg_span.h>
#include <wg_text.h>
#include <wg_value.h>
#include <wg_columnheaderitem.h>
#include <wg_edittextitem.h>
#include <wg_editvalueitem.h>
#include <wg_iconitem.h>
#include <wg_item.h>
#include <wg_modvalueitem.h>
#include <wg_spanitem.h>
#include <wg_textbaseitem.h>
#include <wg_textitem.h>
#include <wg_valueitem.h>
#include <wg_scalepreferred_sizebroker.h>
#include <wg_sizebroker.h>
#include <wg_uniform_sizebroker.h>
#include <wg_blockskin.h>
#include <wg_boxskin.h>
#include <wg_staticcolorskin.h>
#include <wg_extendedskin.h>
#include <wg_skin.h>
#include <wg_stdtextmapper.h>
#include <wg_textmapper.h>
#include <wg_standardformatter.h>
#include <wg_timeformatter.h>
#include <wg_valueformatter.h>
#include <wg_capsule.h>
#include <wg_shadercapsule.h>
#include <wg_sizecapsule.h>
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
#include <wg_oscilloscope.h>
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
