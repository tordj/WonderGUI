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

		virtual void	setContentPadding(const Border& padding);
		virtual Border	contentPadding() const;

		//.____ Rendering ______________________________________________________

		void			setLayer(int layer);
		int				layer() const { return m_layer; }

		//.____ Misc ____________________________________________________

		inline bool		isOpaque() const { return m_bOpaque; }
		inline bool		isOpaque(State state) const { return _isOpaque(state); };

		//.____ Internal ________________________________________________________

		virtual SizeSPX		_minSize(int scale) const;
		virtual SizeSPX		_preferredSize(int scale) const;

		virtual BorderSPX	_contentPadding(int scale, State state) const;
		virtual SizeSPX		_contentPaddingSize(int scale) const;
		virtual SizeSPX		_sizeForContent(const SizeSPX& contentSize, int scale) const;
		virtual CoordSPX	_contentOfs(int scale, State state) const;
		virtual RectSPX		_contentRect(const RectSPX& canvas, int scale, State state ) const;

		bool				_isOpaque() const { return m_bOpaque; }
		virtual bool		_isOpaque(State state) const;
		virtual bool		_isOpaque(const RectSPX& rect, const SizeSPX& canvasSize, int scale, State state) const;

		virtual bool		_markTest(const CoordSPX& ofs, const RectSPX& canvas, int scale, State state, int opacityTreshold,
									float value = 1.f, float value2 = -1.f) const = 0;

		virtual void 		_render(GfxDevice* pDevice, const RectSPX& canvas, int scale, State state,
									float value = 1.f, float value2 = -1.f, int animPos = 0, 
									float* pStateFractions = nullptr) const = 0;

		virtual RectSPX		_dirtyRect(const RectSPX& canvas, int scale, State newState, State oldState, 
									float newValue = 1.f, float oldValue = 1.f, 
									float newValue2 = -1.f, float oldValue2 = -1.f, 
									int newAnimPos = 0, int oldAnimPos = 0,
									float* pNewStateFractions = nullptr, float* pOldStateFractions = nullptr) const;

		inline bool			_ignoresValue() const { return m_bIgnoresValue; }
		inline bool			_ignoresState() const { return m_bIgnoresState; }

		inline bool			_isContentShifting() const { return m_bContentShifting; }


		virtual int			_animationLength(State state) const;

		virtual Bitmask<uint8_t> _transitioningStates() const;
		virtual const int*	_transitionTimes() const;

	protected:
		Skin() {};

        inline void     _doIncUseCount(Skin * pSkin) { pSkin->_incUseCount();}
        inline void     _doDecUseCount(Skin * pSkin) { pSkin->_decUseCount();}

		virtual void	_incUseCount() { m_useCount++; }
		virtual void	_decUseCount() { m_useCount--; }

		Border			m_contentPadding;
		bool			m_bContentShifting = false;
		bool			m_bIgnoresValue = true;
		bool			m_bIgnoresState = true;
		bool			m_bOpaque = false;
		int				m_useCount = 0;						// Counter of instances of this skin in use.
		int				m_layer = -1;
	};




} // namespace wg
#endif //WG_SKIN_DOT_H
