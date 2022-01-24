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

#ifndef	WG_LINEEDITOR_DOT_H
#define	WG_LINEEDITOR_DOT_H
#pragma once


#include <wg_widget.h>
#include <wg_ctexteditor.h>

namespace wg
{

	class LineEditor;
	typedef	StrongPtr<LineEditor>	LineEditor_p;
	typedef	WeakPtr<LineEditor>		LineEditor_wp;

	class LineEditor:public Widget
	{
	public:

		//____ Blueprint ______________________________________________________

		struct Blueprint
		{
			Object_p		baggage;
			bool			dropTarget = false;
			CTextEditor::Blueprint	editor;
			bool			enabled = true;
			Finalizer_p		finalizer;
			int				id = 0;
			MarkPolicy		markPolicy = MarkPolicy::AlphaTest;
			bool			pickable = false;
			int				pickCategory = 0;
			PointerStyle	pointer = PointerStyle::Default;
			int				scale = 64;
			bool			selectable = true;
			Skin_p			skin;
			bool			tabLock = false;
			String			tooltip;
		};

		//.____ Creation __________________________________________

		static LineEditor_p	create() { return LineEditor_p(new LineEditor()); }
		static LineEditor_p	create(const Blueprint& blueprint) { return LineEditor_p(new LineEditor(blueprint)); }

		//.____ Components _______________________________________

		CTextEditor		editor;

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Internal _________________________________________________

		SizeSPX			_preferredSize(int scale = -1) const override;

	protected:
		LineEditor();
		LineEditor(const Blueprint& bp);
		virtual ~LineEditor();


		void			_render( GfxDevice * pDevice, const RectSPX& _canvas, const RectSPX& _window ) override;
		void			_resize( const SizeSPX& size, int scale = -1 ) override;
		void			_refresh() override;
		void			_receive( Msg * pMsg ) override;
		void			_setState( State state ) override;

		CoordSPX		_componentPos( const WidgetComponent * pComponent ) const override;
		SizeSPX			_componentSize( const WidgetComponent * pComponent ) const override;
		RectSPX			_componentGeo( const WidgetComponent * pComponent ) const override;

		void			_componentRequestRender(const WidgetComponent * pComponent) override;
		void			_componentRequestRender( const WidgetComponent * pComponent, const RectSPX& rect ) override;
		void			_componentRequestResize( const WidgetComponent * pComponent ) override;
		void			_componentRequestInView(const WidgetComponent * pComponent, const RectSPX& mustHave, const RectSPX& niceToHave) override;

	private:

		spx				m_textScrollOfs;
	};






} // namespace wg
#endif // WG_TEXTDISPLAY_DOT_H
