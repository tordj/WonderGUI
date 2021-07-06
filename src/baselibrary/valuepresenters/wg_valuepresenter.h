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
#ifndef	WG_VALUEPRESENTER_DOT_H
#define WG_VALUEPRESENTER_DOT_H
#pragma once

#include <wg_object.h>
#include <wg_pointers.h>
#include <wg_string.h>

namespace wg
{

	class ValuePresenter;
	typedef	StrongPtr<ValuePresenter>		ValuePresenter_p;
	typedef	WeakPtr<ValuePresenter>	ValuePresenter_wp;

	//____ ValuePresenter __________________________________________________________

	class ValuePresenter : public Object
	{
	public:

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Misc __________________________________________________

		virtual void 	render(GfxDevice* pDevice, const RectSPX& canvas, double value, int scale, State state) = 0;
		virtual SizeSPX	preferredSize(double value, int scale, State state) const = 0;
		virtual bool	stateChangeNeedsRender(State newState, State oldState) const = 0;

	protected:

		void	_renderString(GfxDevice * pDevice, const Char* pString, TextStyle* pStyle, CoordSPX pos, int scale, State state );

	};


} // namespace wg
#endif //WG_VALUEPRESENTER_DOT_H
