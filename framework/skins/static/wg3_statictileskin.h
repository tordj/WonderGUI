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
#ifndef WG3_STATICTILESKIN_DOT_H
#define WG3_STATICTILESKIN_DOT_H
#pragma once

#include <wg3_skin.h>
#include <wg3_surface.h>

namespace wg
{
	class StaticTileSkin;

	typedef	StrongPtr<StaticTileSkin>	StaticTileSkin_p;
	typedef	WeakPtr<StaticTileSkin>	StaticTileSkin_wp;


	class StaticTileSkin : public Skin
	{
	public:
		//.____ Creation __________________________________________

		static StaticTileSkin_p create(Surface* pSurface);

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Geometry _________________________________________________

		Size	preferredSize() const override;

		//.____ Misc ____________________________________________________

		bool		markTest(	const Coord& ofs, const Rect& canvas, State state, int opacityTreshold, 
								float fraction = 1.f, float fraction2 = -1.f) const override;

		void		render(	GfxDevice * pDevice, const Rect& canvas, State state, 
							float fraction = 1.f, float fraction2 = -1.f) const override;

	private:
		StaticTileSkin(Surface* pSurface);
		~StaticTileSkin() {};

		Surface_p		m_pSurface;
	};


} // namespace wg
#endif //WG3_STATICTILESKIN_DOT_H