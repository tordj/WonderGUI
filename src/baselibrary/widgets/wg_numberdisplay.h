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

#ifndef	WG_NUMBERDISPLAY_DOT_H
#define	WG_NUMBERDISPLAY_DOT_H
#pragma once

#	include <wg_widget.h>
#	include <wg_cnumberdisplay.h>

namespace wg
{


	class NumberDisplay;
	typedef	StrongPtr<NumberDisplay>		NumberDisplay_p;
	typedef	WeakPtr<NumberDisplay>		NumberDisplay_wp;

	/**
	* @brief Widget that displays a formatted value.
	*/
	class NumberDisplay : public Widget
	{
	public:
		//.____ Creation __________________________________________

		static NumberDisplay_p	create() { return NumberDisplay_p(new NumberDisplay()); }

		//.____ Components _______________________________________

		CNumberDisplay		display;

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Internal _________________________________________________

		SizeSPX			_preferredSize(int scale = -1) const override;



	protected:
		NumberDisplay();
		virtual ~NumberDisplay();
		virtual Widget* _newOfMyType() const override { return new NumberDisplay(); };

		void	_refresh() override;
		void	_cloneContent( const Widget * _pOrg ) override;
		void	_render( GfxDevice * pDevice, const RectSPX& _canvas, const RectSPX& _window ) override;
		void	_setState( State state ) override;

	private:

		class DisplayAccess : public CNumberDisplay { friend class NumberDisplay; };
		const DisplayAccess& _display() const { return static_cast<const DisplayAccess&>(display); }
		DisplayAccess& _display() { return static_cast<DisplayAccess&>(display); }

	};

} // namespace wg
#endif // WG_NUMBERDISPLAY_DOT_H
