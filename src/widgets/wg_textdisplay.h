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

		//.____ Geometry _________________________________________________

		MU				matchingWidth(MU height) const override;
		MU				matchingHeight(MU width) const override;
		Size			preferredSize() const override;

		//.____ Appearance _________________________________________________

		PointerStyle	pointerStyle() const override;
		String			tooltip() const override;

	protected:
		TextDisplay();
		virtual ~TextDisplay();
		virtual Widget* _newOfMyType() const override { return new TextDisplay(); };


		void			_cloneContent( const Widget * _pOrg ) override;
		void			_render( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window ) override;
		void			_resize( const Size& size ) override;
		void			_refresh() override;
		void			_receive( Msg * pMsg ) override;
		void			_setState( State state ) override;

	};




} // namespace wg
#endif // WG_TEXTDISPLAY_DOT_H
