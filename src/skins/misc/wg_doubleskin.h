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
#ifndef WG_DOUBLESKIN_DOT_H
#define WG_DOUBLESKIN_DOT_H
#pragma once

#include <wg_skin.h>
#include <wg_color.h>

#include <initializer_list>
#include <utility>

namespace wg
{

	class DoubleSkin;
	typedef	StrongPtr<DoubleSkin>	DoubleSkin_p;
	typedef	WeakPtr<DoubleSkin>		DoubleSkin_wp;

	class DoubleSkin : public Skin
	{
	public:
		//.____ Creation __________________________________________

		static DoubleSkin_p	create();
		static DoubleSkin_p create( Skin * pFrontSkin, Skin * pBackSkin, bool bSkinInSkin = true );

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Appearance _________________________________________________

		bool			setFrontSkin(Skin * pSkin);
		Skin_p			frontSkin() const { return m_pFrontSkin; }

		bool			setBackSkin(Skin * pSkin);
		Skin_p			backSkin() const { return m_pBackSkin; }

		void			setSkinInSkin(bool bInside);
		bool			isSkinInSkin() const { return m_bSkinInSkin; }

		//.____ Geometry _________________________________________________

		Size			minSize() const override;
		Size			preferredSize() const override;
		Size			sizeForContent(const Size& contentSize) const override;

		void			setContentPadding(const BorderI& padding) override;
		Border			contentPadding(State state) const override;
		Size			contentPaddingSize() const override;
		Coord			contentOfs(State state) const override;
		Rect			contentRect(const Rect& canvas, State state) const override;

		//.____ Misc ____________________________________________________

		bool			isOpaque( State state ) const override;
		bool			isOpaque(const Rect& rect, const Size& canvasSize, State state) const override;

		bool			isStateIdentical( State state, State comparedTo, float fraction = 1.f, float fraction2 = -1.f) const override;

		bool			markTest(	const Coord& ofs, const Rect& canvas, State state, int opacityTreshold, 
									float fraction = 1.f, float fraction2 = -1.f) const override;

		void			render(	GfxDevice * pDevice, const Rect& canvas, State state, 
								float fraction = 1.f, float fraction2 = -1.f) const override;

		Rect			fractionChangeRect(	const Rect& canvas, State state, float oldFraction, float newFraction,
											float oldFraction2 = -1.f, float newFraction2 = -1.f) const override;


	private:
		DoubleSkin();
		DoubleSkin(Skin * pFrontSkin, Skin * pBackSkin, bool bSkinInSkin = true);
		~DoubleSkin();

		void		_incUseCount() override;
		void		_decUseCount() override;
		void		_onModified();

		Rect		_subSkinGeo(Skin* pSubSkin, const Rect& myGeo, State state) const override;


		Skin_p		m_pFrontSkin;
		Skin_p		m_pBackSkin;
		bool		m_bSkinInSkin = false;
		bool		m_bContentPaddingSet = false;
	};


} // namespace wg
#endif //WG_BOXSKIN_DOT_H


