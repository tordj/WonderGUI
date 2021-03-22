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
#ifndef WG_INTERNAL_DOT_H
#define WG_INTERNAL_DOT_H
#pragma once

#include <wg_container.h>
#include <wg_sidecanvas.h>
#include <wg_ctexteditor.h>
#include <wg_ccanvas.h>
#include <wg_cvaluedisplay.h>
#include <wg_skin.h>

namespace wg
{

	class OWidget : public Widget
	{
	public:
		using Widget::_resize;
		using Widget::_collectPatches;
		using Widget::_maskPatches;
		using Widget::_render;
		using Widget::_parent;
		using Widget::_slot;
		using Widget::_windowPadding;
		using Widget::_preRender;
		using Widget::_setState;
	};

	class OSideCanvas : public SideCanvas
	{
	public:
		using SideCanvas::_contentRect;
		using SideCanvas::_contentPaddingSize;
		using SideCanvas::_requestResize;

	};


	class OContainer : public Container
	{
	public:
		using Widget::_resize;
		using Widget::_collectPatches;
		using Widget::_maskPatches;
		using Widget::_render;
		using Widget::_parent;
		using Widget::_slot;
		using Widget::_windowPadding;
		using Widget::_preRender;
		using Widget::_setState;


		using Container::_findWidget;
		using Container::_getPopupLayer;
		using Container::_getModalLayer;
	};

	class OSlot : public StaticSlot
	{
	public:
		using StaticSlot::_widget;
		using StaticSlot::_setWidget;
		using StaticSlot::_setSize;

	};

	class OGeoComponent : public GeoComponent
	{
	public:
	};

	class OCCanvas : public CCanvas
	{
	public:
		using CCanvas::_render;
		using CCanvas::_alphaTest;

		using CCanvas::_regenSurface;
		using CCanvas::_calcPresentationArea;

		using CCanvas::_setComponentSize;
		using CCanvas::_preferredSize;
	};

	class OCStaticTextDisplay : public CStaticTextDisplay
	{
	public:
		using CStaticTextDisplay::_clear;

		using CStaticTextDisplay::_set;

		using CStaticTextDisplay::_append;
		using CStaticTextDisplay::_insert;
		using CStaticTextDisplay::_replace;
		using CStaticTextDisplay::_erase;

		using CStaticTextDisplay::_setCharStyle;
		using CStaticTextDisplay::_clearCharStyle;

		using CStaticTextDisplay::_getString;

		using CStaticTextDisplay::_state;
		using CStaticTextDisplay::_setState;

		using CStaticTextDisplay::_setSize;
		using CStaticTextDisplay::_size;

		using CStaticTextDisplay::_preferredSize;
		using CStaticTextDisplay::_matchingWidth;
		using CStaticTextDisplay::_matchingHeight;

		using CStaticTextDisplay::_charAtPos;
		using CStaticTextDisplay::_charRect;
		using CStaticTextDisplay::_charLine;

		using CStaticTextDisplay::_refresh;

		using CStaticTextDisplay::_tooltip;

		using CStaticTextDisplay::_render;

		using CStaticTextDisplay::_rectForRange;

		using CStaticTextDisplay::_textMapper;
		using CStaticTextDisplay::_style;

		using CStaticTextDisplay::_receive;
	};


	class OCTextDisplay : public CTextDisplay
	{
	public:
		using CTextDisplay::_clear;

		using CTextDisplay::_set;

		using CTextDisplay::_append;
		using CTextDisplay::_insert;
		using CTextDisplay::_replace;
		using CTextDisplay::_erase;

		using CTextDisplay::_setCharStyle;
		using CTextDisplay::_clearCharStyle;

		using CTextDisplay::_getString;

		using CTextDisplay::_state;
		using CTextDisplay::_setState;

		using CTextDisplay::_setSize;
		using CTextDisplay::_size;

		using CTextDisplay::_preferredSize;
		using CTextDisplay::_matchingWidth;
		using CTextDisplay::_matchingHeight;

		using CTextDisplay::_charAtPos;
		using CTextDisplay::_charRect;
		using CTextDisplay::_charLine;

		using CTextDisplay::_refresh;

		using CTextDisplay::_tooltip;

		using CTextDisplay::_render;

		using CTextDisplay::_rectForRange;

		using CTextDisplay::_textMapper;
		using CTextDisplay::_style;

		using CTextDisplay::_receive;
	};

	class OCTextEditor : public CTextEditor
	{
	public:
		using CTextEditor::_style;
		using CTextEditor::_preferredSize;
		using CTextEditor::_matchingHeight;
		using CTextEditor::_matchingWidth;
		using CTextEditor::_render;
		using CTextEditor::_refresh;
		using CTextEditor::_setSize;

		using CTextEditor::_receive;
		using CTextEditor::_clear;
		using CTextEditor::_set;

		using CTextEditor::_append;
		using CTextEditor::_insert;
		using CTextEditor::_replace;
		using CTextEditor::_erase;

		using CTextEditor::_setState;
	};

	class OCValueDisplay : public CValueDisplay
	{
	public:
		using CValueDisplay::_preferredSize;
		using CValueDisplay::_refresh;
		using CValueDisplay::_render;
		using CValueDisplay::_setState;
	};


	class OSkin : public Skin
	{
	public:
		using Skin::_incUseCount;
		using Skin::_decUseCount;
	};


	inline OWidget *			OO(Widget* pWidget) { return reinterpret_cast<OWidget*>(pWidget); }
	inline const OWidget *		OO(const Widget* pWidget) { return reinterpret_cast<const OWidget*>(pWidget); }

	inline OContainer *			OO(Container* pContainer) { return reinterpret_cast<OContainer*>(pContainer); }
	inline const OContainer *	OO(const Container* pContainer) { return reinterpret_cast<const OContainer*>(pContainer); }

	inline OSlot *				OO(StaticSlot* pSlot) { return reinterpret_cast<OSlot*>(pSlot); }
	inline const OSlot *		OO(const StaticSlot* pSlot) { return reinterpret_cast<const OSlot*>(pSlot); }

	inline OSlot&				OO(StaticSlot& slot) { return reinterpret_cast<OSlot&>(slot); }
	inline const OSlot&			OO(const StaticSlot& slot) { return reinterpret_cast<const OSlot&>(slot); }

	inline OGeoComponent&		OO(GeoComponent& component) { return reinterpret_cast<OGeoComponent&>(component); }
	inline const OGeoComponent&	OO(const GeoComponent& component) { return reinterpret_cast<const OGeoComponent&>(component); }

	inline OCCanvas&			OO(CCanvas& component) { return reinterpret_cast<OCCanvas&>(component); }
	inline const OCCanvas&		OO(const CCanvas& component) { return reinterpret_cast<const OCCanvas&>(component); }

	inline OCStaticTextDisplay&	OO(CStaticTextDisplay& component) { return reinterpret_cast<OCStaticTextDisplay&>(component); }
	inline const OCStaticTextDisplay&	OO(const CStaticTextDisplay& component) { return reinterpret_cast<const OCStaticTextDisplay&>(component); }

	inline OCTextDisplay&		OO(CTextDisplay& component) { return reinterpret_cast<OCTextDisplay&>(component); }
	inline const OCTextDisplay&	OO(const CTextDisplay& component) { return reinterpret_cast<const OCTextDisplay&>(component); }

	inline OCTextEditor&		OO(CTextEditor& component) { return reinterpret_cast<OCTextEditor&>(component); }
	inline const OCTextEditor&	OO(const CTextEditor& component) { return reinterpret_cast<const OCTextEditor&>(component); }

	inline OCValueDisplay&		OO(CValueDisplay& component) { return reinterpret_cast<OCValueDisplay&>(component); }
	inline const OCValueDisplay& OO(const CValueDisplay& component) { return reinterpret_cast<const OCValueDisplay&>(component); }

	inline OSideCanvas *		OO(SideCanvas* pWidget) { return reinterpret_cast<OSideCanvas*>(pWidget); }
	inline const OSideCanvas *	OO(const SideCanvas* pWidget) { return reinterpret_cast<const OSideCanvas*>(pWidget); }

	inline OSkin* OO(Skin* pSkin) { return reinterpret_cast<OSkin*>(pSkin); }
	inline const OSkin* OO(const Skin* pSkin) { return reinterpret_cast<const OSkin*>(pSkin); }

}

#endif //WG_INTERNAL_DOT_H
