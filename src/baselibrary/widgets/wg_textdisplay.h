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
#include <wg_text.h>

namespace wg
{

	class TextDisplay;
	typedef	StrongPtr<TextDisplay>		TextDisplay_p;
	typedef	WeakPtr<TextDisplay>	TextDisplay_wp;

	class TextDisplay:public Widget
	{
	public:

		//____ Blueprint ______________________________________________________

		struct Blueprint
		{
			Object_p		baggage;
			bool			dropTarget = false;
			Text::Blueprint	display;
			bool			enabled = true;
			Finalizer_p		finalizer = nullptr;
			int				id = 0;
			MarkPolicy		markPolicy = MarkPolicy::AlphaTest;
			bool			pickable = false;
			int				pickCategory = 0;
			PointerStyle	pointer = PointerStyle::Default;
			int				scale = -1;
			bool			selectable = true;
			Skin_p			skin;
			bool			tabLock = false;
			String			tooltip;
		};

		//.____ Creation __________________________________________

		static TextDisplay_p	create() { return TextDisplay_p(new TextDisplay()); }
		static TextDisplay_p	create(const Blueprint& blueprint) { return TextDisplay_p(new TextDisplay(blueprint)); }

		//.____ Components ____________________________________

		Text			display;

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Appearance _________________________________________________

		PointerStyle	pointerStyle() const override;
		String			tooltip() const override;

		//.____ Internal _________________________________________________

		spx				_matchingWidth(spx height, int scale = -1) const override;
		spx				_matchingHeight(spx width, int scale = -1) const override;
		SizeSPX			_defaultSize(int scale = -1) const override;


	protected:
		TextDisplay();
		TextDisplay(const Blueprint& bp);
		virtual ~TextDisplay();


		void			_render( GfxDevice * pDevice, const RectSPX& _canvas, const RectSPX& _window ) override;
		void			_resize( const SizeSPX& size, int scale = -1 ) override;
		void			_receive( Msg * pMsg ) override;
		void			_setState( State state ) override;

	};




} // namespace wg
#endif // WG_TEXTDISPLAY_DOT_H
