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
#ifndef WG_SKIN_DOT_H
#define WG_SKIN_DOT_H
#pragma once

#include <wg_pointers.h>
#include <wg_geo.h>
#include <wg_types.h>
#include <wg_util.h>

namespace wg
{

	class Skin;
	class GfxDevice;
	class SkinSlot;

	typedef	StrongPtr<Skin>	Skin_p;
	typedef	WeakPtr<Skin>	Skin_wp;


	class Skin : public Object
	{
		friend class SkinSlot;

	public:
		virtual ~Skin() {};

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Geometry _________________________________________________

		virtual Size	minSize() const;
		virtual Size	preferredSize() const;

		virtual void	setContentPadding(const BorderI& padding);
		virtual Border	contentPadding(State state) const;

		virtual Size	contentPaddingSize() const;
		virtual Size	sizeForContent(const Size& contentSize) const;
		virtual Coord	contentOfs(State state) const;
		virtual Rect	contentRect(const Rect& canvas, State state) const;

		//.____ Misc ____________________________________________________

		bool			isOpaque() const { return m_bOpaque; }
		virtual bool	isOpaque( State state ) const;
		virtual bool	isOpaque( const Rect& rect, const Size& canvasSize, State state ) const;

		virtual bool	isStateIdentical( State state, State comparedTo, float fraction = 1.f, float fraction2 = -1.f) const;

		virtual bool	markTest(	const Coord& ofs, const Rect& canvas, State state, int opacityTreshold, 
									float fraction = 1.f, float fraction2 = -1.f ) const = 0;

		virtual void 	render(	GfxDevice * pDevice, const Rect& canvas, State state, 
								float fraction = 1.f, float fraction2 = -1.f ) const = 0;

		inline bool		ignoresFraction() const { return m_bIgnoresFraction; }

		virtual Rect	fractionChangeRect(	const Rect& canvas, State state, float oldFraction, float newFraction, 
											float oldFraction2 = -1.f, float newFraction2 = -1.f ) const;

		inline bool		isContentShifting() { return m_bContentShifting; }

		//.____ Deprecated ______________________________________________________
/*
		inline bool		_markTest(const CoordI& ofs, const RectI& canvas, State state, int opacityTreshold) const
		{
			return markTest(reinterpret_cast<const Coord&>(ofs), reinterpret_cast<const Rect&>(canvas), state, opacityTreshold);
		}

		inline bool		_isOpaque(const RectI& rect, const SizeI& canvasSize, State state) const
		{
			return isOpaque(reinterpret_cast<const Rect&>(rect), reinterpret_cast<const Size&>(canvasSize), state);
		}

		inline SizeI	_minSize() const
		{
			return minSize().qpix();
		}

		inline SizeI	_preferredSize() const
		{
			return preferredSize().qpix();
		}

		inline SizeI	_sizeForContent(const SizeI contentSize) const
		{
			return sizeForContent(Util::qpixToMU(contentSize)).qpix();
		}

		inline BorderI	_contentPadding() const
		{
			return contentPadding().qpix();
		}

		inline SizeI	_contentPaddingSize() const
		{
			return contentPaddingSize().qpix();
		}

		inline CoordI	_contentOfs(State state) const
		{
			return contentOfs(state).qpix();
		}

		inline RectI	_contentRect(const RectI& canvas, State state) const
		{
			return contentRect(Util::qpixToMU(canvas), state).qpix();
		}

		inline void 	_render(GfxDevice * pDevice, const RectI& canvas, State state) const
		{
			render(pDevice, Util::qpixToMU(canvas), state);
		}
*/
	protected:
		Skin() {};

		virtual void	_addSlot(SkinSlot* pSlot);
		virtual void	_removeSlot(SkinSlot* pSlot);

		virtual Rect	_subSkinGeo(Skin* pSubSkin, const Rect& myGeo, State state) const;
		bool			_setSuperSkin(Skin* pSuperSkin);
		Skin*			_superSkin() const { return m_pSuperSkin; }

		BorderI		m_contentPadding;					// Unit: Points
		bool		m_bContentShifting = false;
		bool		m_bIgnoresFraction = true;
		bool		m_bOpaque = false;
		int			m_nSkinSlots = 0;					// Counter of connected SkinSlots.
		Skin*		m_pSuperSkin = nullptr;				// Skin containing us, affecting our geo (there can only be one).
	};




} // namespace wg
#endif //WG_SKIN_DOT_H
