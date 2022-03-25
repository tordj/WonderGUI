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

#ifndef	WG_TEXTEDITOR_DOT_H
#define	WG_TEXTEDITOR_DOT_H
#pragma once


#include <wg_widget.h>
#include <wg_ctexteditor.h>

namespace wg
{

	class TextEditor;
	typedef	StrongPtr<TextEditor>		TextEditor_p;
	typedef	WeakPtr<TextEditor>	TextEditor_wp;

	/**
	* @brief Editable text widget.
	*/
	class TextEditor:public Widget
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

		static TextEditor_p	create() { return TextEditor_p(new TextEditor()); }
		static TextEditor_p	create(const Blueprint& blueprint) { return TextEditor_p(new TextEditor(blueprint)); }

		//.____ Components ____________________________________

		CTextEditor		editor;

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Internal _________________________________________________

		spx				_matchingWidth(spx width, int scale = -1) const override;
		spx				_matchingHeight(spx width, int scale = -1) const override;
		SizeSPX			_defaultSize(int scale = -1) const override;

	protected:
		TextEditor();
		TextEditor(const Blueprint& bp);
		virtual ~TextEditor();

		void			_render( GfxDevice * pDevice, const RectSPX& _canvas, const RectSPX& _window ) override;
		void			_resize( const SizeSPX& size, int scale = -1 ) override;
		void			_refresh() override;
		void			_receive( Msg * pMsg ) override;
		void			_setState( State state ) override;
	};

} // namespace wg
#endif // WG_TEXTEDITOR_DOT_H
