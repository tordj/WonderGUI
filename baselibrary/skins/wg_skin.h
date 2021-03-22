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
	class CSkinSlot;

	typedef	StrongPtr<Skin>	Skin_p;
	typedef	WeakPtr<Skin>	Skin_wp;


	class Skin : public Object
	{
		friend class CSkinSlot;
		friend class CSkinSlotMI;

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

		//.____ Rendering ______________________________________________________

		void			setLayer(int layer);
		int				layer() const { return m_layer; }

		//.____ Misc ____________________________________________________

		bool			isOpaque() const { return m_bOpaque; }
		virtual bool	isOpaque( State state ) const;
		virtual bool	isOpaque( const Rect& rect, const Size& canvasSize, State state ) const;

		virtual bool	markTest(	const Coord& ofs, const Rect& canvas, State state, int opacityTreshold, 
									float value = 1.f, float value2 = -1.f ) const = 0;

		virtual void 	render(	GfxDevice * pDevice, const Rect& canvas, State state, 
								float value = 1.f, float value2 = -1.f, int animPos = 0, float* pStateFractions = nullptr) const = 0;

		inline bool		ignoresValue() const { return m_bIgnoresValue; }
		inline bool		ignoresState() const { return m_bIgnoresState; }

		inline bool		isContentShifting() const { return m_bContentShifting; }

		virtual Rect	dirtyRect(	const Rect& canvas, State newState, State oldState, float newValue = 1.f, float oldValue = 1.f,
									float newValue2 = -1.f, float oldValue2 = -1.f, int newAnimPos = 0, int oldAnimPos = 0, 
									float* pNewStateFractions = nullptr, float* pOldStateFractions = nullptr) const;

		virtual int		animationLength(State state) const;

		virtual Bitmask<uint8_t>transitioningStates() const;
		virtual const int*		transitionTimes() const;

	protected:
		Skin() {};

        inline void     _doIncUseCount(Skin * pSkin) { pSkin->_incUseCount();}
        inline void     _doDecUseCount(Skin * pSkin) { pSkin->_decUseCount();}

		virtual void	_incUseCount() { m_useCount++; }
		virtual void	_decUseCount() { m_useCount--; }

		BorderI			m_contentPadding;					// Unit: Points
		bool			m_bContentShifting = false;
		bool			m_bIgnoresValue = true;
		bool			m_bIgnoresState = true;
		bool			m_bOpaque = false;
		int				m_useCount = 0;						// Counter of instances of this skin in use.
		int				m_layer = -1;
	};




} // namespace wg
#endif //WG_SKIN_DOT_H
