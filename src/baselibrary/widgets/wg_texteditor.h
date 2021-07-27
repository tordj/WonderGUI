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
		//.____ Creation __________________________________________

		static TextEditor_p	create() { return TextEditor_p(new TextEditor()); }

		//.____ Components ____________________________________

		CTextEditor		text;

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Internal _________________________________________________

		spx				_matchingHeight(spx width, int scale = -1) const override;
		SizeSPX			_preferredSize(int scale = -1) const override;

	protected:
		TextEditor();
		virtual ~TextEditor();
		virtual Widget* _newOfMyType() const override { return new TextEditor(); };

		void			_cloneContent( const Widget * _pOrg ) override;
		void			_render( GfxDevice * pDevice, const RectSPX& _canvas, const RectSPX& _window ) override;
		void			_resize( const SizeSPX& size, int scale = -1 ) override;
		void			_refresh() override;
		void			_receive( Msg * pMsg ) override;
		void			_setState( State state ) override;
	};

} // namespace wg
#endif // WG_TEXTEDITOR_DOT_H
