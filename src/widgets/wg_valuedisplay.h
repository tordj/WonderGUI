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

#ifndef	WG_VALUEDISPLAY_DOT_H
#define	WG_VALUEDISPLAY_DOT_H
#pragma once

#	include <wg_widget.h>
#	include <wg_cvaluedisplay.h>

namespace wg
{


	class ValueDisplay;
	typedef	StrongPtr<ValueDisplay>		ValueDisplay_p;
	typedef	WeakPtr<ValueDisplay>		ValueDisplay_wp;

	/**
	* @brief Widget that displays a formatted value.
	*/
	class ValueDisplay : public Widget
	{
	public:
		//.____ Creation __________________________________________

		static ValueDisplay_p	create() { return ValueDisplay_p(new ValueDisplay()); }

		//.____ Components _______________________________________

		CValueDisplay		value;

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Geometry _________________________________________________

		Size			preferredSize() const override;


	protected:
		ValueDisplay();
		virtual ~ValueDisplay();
		virtual Widget* _newOfMyType() const override { return new ValueDisplay(); };

		void	_refresh() override;
		void	_cloneContent( const Widget * _pOrg ) override;
		void	_render( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window ) override;
		void	_setState( State state ) override;
		void	_setSkin( Skin * pSkin ) override;

	private:

		class ValueAccess : public CValueDisplay { friend class ValueDisplay; };
		const ValueAccess& _value() const { return static_cast<const ValueAccess&>(value); }
		ValueAccess& _value() { return static_cast<ValueAccess&>(value); }

	};

} // namespace wg
#endif // WG_VALUEDISPLAY_DOT_H
