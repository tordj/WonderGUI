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
#ifndef WG_BAKESKIN_DOT_H
#define WG_BAKESKIN_DOT_H
#pragma once

#include <wg_cdynamicvector.h>
#include <wg_surface.h>
#include <wg_skin.h>
#include <wg_color.h>
#include <wg_gradient.h>

#include <initializer_list>
#include <utility>

namespace wg
{
	class BakeSkin;
	typedef	StrongPtr<BakeSkin>	BakeSkin_p;
	typedef	WeakPtr<BakeSkin>		BakeSkin_wp;

	class BakeSkin : public Skin, protected CDynamicVector<Skin_p>::Holder
	{
	public:
		//.____ Creation __________________________________________

		static BakeSkin_p	create();
		static BakeSkin_p	create(Surface* pBakeSurface);
		static BakeSkin_p	create( Surface * pBakeSurface, std::initializer_list<Skin_p> skins );

		//.____ Components ____________________________________

		CDynamicVector<Skin_p>	skins;

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Appearance _________________________________________________

		void			setBlendMode( BlendMode blend );
		BlendMode		blendMode() const { return m_blendMode; }

		void			setColor(HiColor color);
		HiColor			color() const { return m_tintColor; }

		void			setGradient(const Gradient& gradient);
		Gradient		gradient() const { return m_gradient; }


		void			setSkinInSkin(bool bInside);
		bool			isSkinInSkin() const { return m_bSkinInSkin; }

		//.____ Geometry _________________________________________________

		void			setContentPadding(const Border& padding) override;

		//.____ Misc ____________________________________________________

		void			setBakeSurface(Surface* pSurface);
		Surface_p		bakeSurface() const { return m_pBakeSurface; }

		//.____ Internal _________________________________________________

		SizeSPX			_minSize(int scale) const override;
		SizeSPX			_preferredSize(int scale) const override;

		BorderSPX		_contentPadding(int scale, State state) const override;
		SizeSPX			_contentPaddingSize(int scale) const override;
		CoordSPX		_contentOfs(int scale, State state) const override;
		RectSPX			_contentRect(const RectSPX& canvas, int scale, State state) const override;

		bool			_isOpaque( State state ) const override;
		bool			_isOpaque(const RectSPX& rect, const SizeSPX& canvasSize, int scale, State state) const override;

		bool			_markTest(	const CoordSPX& ofs, const RectSPX& canvas, int scale, State state, int opacityTreshold, 
									float value = 1.f, float value2 = -1.f) const override;

		void			_render(	GfxDevice * pDevice, const RectSPX& canvas, int scale, State state, 
									float value = 1.f, float value2 = -1.f, int animPos = 0, float* pStateFractions = nullptr) const override;

		RectSPX			_dirtyRect(	const RectSPX& canvas, int scale, State newState, State oldState, float newValue = 1.f, float oldValue = 1.f,
									float newValue2 = -1.f, float oldValue2 = -1.f, int newAnimPos = 0, int oldAnimPos = 0,
									float* pNewStateFractions = nullptr, float* pOldStateFractions = nullptr) const override;

		int				_animationLength(State state) const override;

		Bitmask<uint8_t>_transitioningStates() const override;
		const int*		_transitionTimes() const override;


	private:
		BakeSkin(Surface * pBakeSurface);
		BakeSkin(Surface* pBakeSurface, std::initializer_list<Skin_p> skins);
		~BakeSkin();

		void			_incUseCount() override;
		void			_decUseCount() override;

		void			_updateCachedGeo(int scale) const;
		void			_onModified();
		BorderSPX		_stateContentPadding(int scale, State state) const;

		void			_didAddEntries(Skin_p* pEntry, int nb) override;
		void			_didMoveEntries(Skin_p* pFrom, Skin_p* pTo, int nb) override;
		void			_willEraseEntries(Skin_p* pEntry, int nb) override;

		Object*			_object() override { return this; }


		Surface_p			m_pBakeSurface;
		BlendMode			m_blendMode = BlendMode::Undefined;
		HiColor				m_tintColor = Color::White;
		Gradient			m_gradient;
		bool				m_bContentPaddingSet = false;
		bool				m_bSkinInSkin = false;

		mutable int			m_cachedScale = 0;
		mutable SizeSPX		m_cachedMinSize;						// Calculated minSize for scale represented by m_cachedScale;
		mutable SizeSPX		m_cachedPreferredSize;					// Calculated preferredSize for scale represented by m_cachedScale;
		mutable BorderSPX	m_cachedContentPadding[StateEnum_Nb];

		Bitmask<uint32_t>	m_opaqueStates;

		Bitmask<uint8_t>	m_transitioningStates;
		int					m_transitionTimes[StateBits_Nb];

		int					m_animationLengths[StateEnum_Nb];
	};

} // namespace wg
#endif //WG_BAKESKIN_DOT_H


