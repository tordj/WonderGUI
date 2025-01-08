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

#ifndef	WG_THEME_DOT_H
#define WG_THEME_DOT_H
#pragma once

#include <wg_pointers.h>

#include <wg_togglebutton.h>

namespace wg
{

	class Theme;
	typedef	StrongPtr<Theme>	Theme_p;
	typedef	WeakPtr<Theme>		Theme_wp;


	class Theme : public Object
	{
	public:

		//.____ Identification __________________________________________

		const TypeInfo& typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Content _______________________________________________________

		virtual const ToggleButton::Blueprint& radioButton() const = 0;


	protected:
		Theme() {};
		~Theme() {}


	};


} // namespace wg
#endif //WG_THEME_DOT_H
