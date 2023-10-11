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
#include <wg_state.h>
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
		friend class SkinSlotMI;

	public:

		//.____ Blueprint __________________________________________

		struct Blueprint
		{
			Finalizer_p	finalizer = nullptr;
			int			layer = -1;
			Border		margin;
			int			markAlpha = 1;
			Border		overflow;
			Border		padding;
		};


		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Geometry _________________________________________________
		
//		virtual Size		defaultSize() const;

		//TODO: Replace this quick n dirty implementation with a correct one.

		inline Size			defaultSize() const { return Size(_defaultSize(64))/64.f; }
		
		//.____ Misc ____________________________________________________

		inline bool		isOpaque() const { return m_bOpaque; }
		inline bool		isOpaque(State state) const { return _isOpaque(state); };

		//.____ Internal ________________________________________________________

		virtual SizeSPX		_minSize(int scale) const;
		virtual SizeSPX		_defaultSize(int scale) const;

		inline  bool		_hasMargin() const { return m_margin.isEmpty();  }
		virtual BorderSPX	_margin(int scale, State state) const;
		virtual SizeSPX		_marginSize(int scale) const;
		
		inline  bool		_hasPadding() const { return m_padding.isEmpty();  }
		virtual BorderSPX	_padding(int scale, State state) const;
		virtual SizeSPX		_paddingSize(int scale) const;

		virtual SizeSPX		_sizeForContent(const SizeSPX& contentSize, int scale) const;
		virtual CoordSPX	_contentOfs(int scale, State state) const;
		virtual RectSPX		_contentRect(const RectSPX& canvas, int scale, State state ) const;
		virtual BorderSPX	_contentBorder(int scale, State state) const;
		virtual SizeSPX		_contentBorderSize(int scale) const;

		bool				_isOpaque() const { return m_bOpaque; }
		virtual bool		_isOpaque(State state) const;
		virtual bool		_isOpaque(const RectSPX& rect, const SizeSPX& canvasSize, int scale, State state) const;

		virtual bool		_markTest(const CoordSPX& ofs, const RectSPX& canvas, int scale, State state,
									float value = 1.f, float value2 = -1.f, int alphaOverride = -1 ) const = 0;

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
		template<class BP> Skin(const BP& bp)
		{
			m_layer		= bp.layer;
			m_margin	= bp.margin;
			m_padding	= bp.padding;
			m_markAlpha = bp.markAlpha;
			m_overflow	= bp.overflow;

			if (bp.finalizer)
				setFinalizer(bp.finalizer);
		}

		virtual ~Skin() {};

        inline void     _doIncUseCount(Skin * pSkin) { pSkin->_incUseCount();}
        inline void     _doDecUseCount(Skin * pSkin) { pSkin->_decUseCount();}

		virtual void	_incUseCount() { m_useCount++; }
		virtual void	_decUseCount() { m_useCount--; }

		Border			m_overflow;
		int				m_markAlpha = 1;

		Border			m_margin;
		Border			m_padding;
		bool			m_bContentShifting = false;
		bool			m_bIgnoresValue = true;
		bool			m_bIgnoresState = true;
		bool			m_bOpaque = false;
		int				m_useCount = 0;						// Counter of instances of this skin in use.
		int				m_layer = -1;
	};

} // namespace wg
#endif //WG_SKIN_DOT_H
