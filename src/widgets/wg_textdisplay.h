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

#ifndef	WG_TEXTDISPLAY_DOT_H
#define	WG_TEXTDISPLAY_DOT_H
#pragma once


#include <wg_widget.h>
#include <wg_itextdisplay.h>

namespace wg
{

	class TextDisplay;
	typedef	StrongPtr<TextDisplay>		TextDisplay_p;
	typedef	WeakPtr<TextDisplay>	TextDisplay_wp;

	class TextDisplay:public Widget
	{
	public:

		//.____ Creation __________________________________________

		static TextDisplay_p	create() { return TextDisplay_p(new TextDisplay()); }

		//.____ Interfaces ____________________________________

		ITextDisplay			text;

		//.____ Identification __________________________________________

		bool					isInstanceOf( const char * pClassName ) const override;
		const char *			className( void ) const override;
		static const char		CLASSNAME[];
		static TextDisplay_p	cast( Object * pObject );

		//.____ Appearance _________________________________________________

		PointerStyle	pointerStyle() const override;
		String			tooltip() const override;


	protected:
		TextDisplay();
		virtual ~TextDisplay();
		virtual Widget* _newOfMyType() const override { return new TextDisplay(); };

		int				_matchingWidth(int height) const override;
		int				_matchingHeight(int width) const override;
		SizeI			_preferredSize() const override;

		void			_cloneContent( const Widget * _pOrg ) override;
		void			_render( GfxDevice * pDevice, const RectI& _canvas, const RectI& _window ) override;
		void			_setSize( const SizeI& size ) override;
		void			_refresh() override;
		void			_receive( Msg * pMsg ) override;
		void			_setState( State state ) override;
		void			_setSkin( Skin * pSkin ) override;

	private:

		CTextDisplay		m_text;
	};




} // namespace wg
#endif // WG_TEXTDISPLAY_DOT_H
