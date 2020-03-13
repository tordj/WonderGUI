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

		bool				isInstanceOf( const char * pClassName ) const override;
		const char *		className( void ) const override;
		static const char	CLASSNAME[];

	protected:
		TextEditor();
		virtual ~TextEditor();
		virtual Widget* _newOfMyType() const override { return new TextEditor(); };

		int				_matchingHeight(int width) const override;
		SizeI			_preferredSize() const override;

		void			_cloneContent( const Widget * _pOrg ) override;
		void			_render( GfxDevice * pDevice, const RectI& _canvas, const RectI& _window ) override;
		void			_resize( const SizeI& size ) override;
		void			_refresh() override;
		void			_receive( Msg * pMsg ) override;
		void			_setState( State state ) override;
		void			_setSkin( Skin * pSkin ) override;

		class TextAccess : public CTextEditor { friend class TextEditor; };
		const TextAccess& _text() const { return static_cast<const TextAccess&>(text); }
		TextAccess& _text() { return static_cast<TextAccess&>(text); }

	};






} // namespace wg
#endif // WG_TEXTEDITOR_DOT_H
