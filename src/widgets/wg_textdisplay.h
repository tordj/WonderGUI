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
#include <wg_ctextdisplay.h>

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

		//.____ Components ____________________________________

		CTextDisplay			text;

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

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
		void			_resize( const SizeI& size ) override;
		void			_refresh() override;
		void			_receive( Msg * pMsg ) override;
		void			_setState( State state ) override;
		void			_setSkin( Skin * pSkin ) override;

		class TextAccess : public CTextDisplay { friend class TextDisplay; };
		const TextAccess& _text() const { return static_cast<const TextAccess&>(text); }
		TextAccess& _text() { return static_cast<TextAccess&>(text); }

	};




} // namespace wg
#endif // WG_TEXTDISPLAY_DOT_H
