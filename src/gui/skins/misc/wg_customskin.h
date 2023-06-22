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
#ifndef WG_CUSTOMSKIN_DOT_H
#define WG_CUSTOMSKIN_DOT_H
#pragma once

#include <wg_skin.h>
#include <functional>

namespace wg
{

	class CustomSkin;

	typedef	StrongPtr<CustomSkin>	CustomSkin_p;
	typedef	WeakPtr<CustomSkin>	CustomSkin_wp;


	class CustomSkin : public Skin
	{
	public:

		typedef std::function<bool(const CoordSPX&,const RectSPX&,int,State,float,float,int)> MarkTestFunc;
		typedef std::function<void(GfxDevice*,const RectSPX&,int,State,float,float)> RenderFunc;

		//____ Blueprint ______________________________________________________

		struct Blueprint
		{
			MarkTestFunc	markTestFunc;
			bool			opaque = false;
			Border			overflow;
			Border			padding;
			RenderFunc		renderFunc;
		};

		//.____ Creation __________________________________________

		static CustomSkin_p create( const Blueprint& blueprint );

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Internal ____________________________________________________

		bool		_markTest(	const CoordSPX& ofs, const RectSPX& canvas, int scale, State state,
								float value = 1.f, float value2 = -1.f, int alphaOverride = -1) const override;

		void		_render(	GfxDevice * pDevice, const RectSPX& canvas, int scale, State state,
							float value = 1.f, float value2 = -1.f, int animPos = 0, float* pStateFractions = nullptr) const override;

	private:
		CustomSkin( const Blueprint& blueprint );
		~CustomSkin() {};

		MarkTestFunc	m_markTestFunc;
		RenderFunc		m_renderFunc;
	};


} // namespace wg
#endif //WG_CUSTOMSKIN_DOT_H
