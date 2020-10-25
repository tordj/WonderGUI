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

#ifndef	WG_CSKINSLOTMI_DOT_H
#define	WG_CSKINSLOTMI_DOT_H
#pragma once

#include <wg_skin.h>
#include <wg_component.h>
#include <wg_skinslotmanager.h>

namespace wg
{
	class CSkinSlotMI;
	typedef	StrongComponentPtr<CSkinSlotMI>	CSkinSlotMI_p;
	typedef	WeakComponentPtr<CSkinSlotMI>	CSkinSlotMI_wp;

	class CSkinSlotMI : public Component, protected SkinSlotPocket::Holder
	{
	public:
		class Holder
		{
		public:
			virtual Object* _object() = 0;
			virtual const Object* _object() const = 0;

			virtual void	_skinChanged(const CSkinSlotMI* pSlot, Skin* pNewSkin, Skin* pOldSkin) = 0;

			virtual State	_skinInstanceState(void* instance) = 0;
			virtual Size	_skinInstanceSize(void* instance) = 0;
			virtual void	_skinInstanceRequestRender(void* instance) = 0;
			virtual void	_skinInstanceRequestRender(void* instance, const Rect& rect) = 0;
			virtual float	_skinInstanceValue(void* instance) = 0;
			virtual float	_skinInstanceValue2(void* instance) = 0;
		};

		CSkinSlotMI(Holder* pHolder) : m_pHolder(pHolder) {}
		~CSkinSlotMI() { if (m_pSkin) m_pSkin->_decUseCount(); }

		//.____ Identification _________________________________________________

		const TypeInfo& typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Content _______________________________________________________

		virtual void	set(Skin* pSkin);
		inline bool		isEmpty() const { return m_pSkin == nullptr; }

		//.____ Misc __________________________________________________

		inline CSkinSlotMI_p	ptr() { return CSkinSlotMI_p(this); }

		//.____ Operators __________________________________________

		inline const Skin* operator->() const { return m_pSkin; }
		inline operator Skin* () const { return m_pSkin; }

		inline CSkinSlotMI& operator=(Skin* pSkin) { set(pSkin); return *this; }
		inline operator Skin_p() const { return m_pSkin; }

	protected:

		// Methods for holder

		void			_addInstance(SkinSlotPocket*& pPocket, void* instance, State state);
		inline void		_removeInstance(SkinSlotPocket*& pPocket) { _releasePocket(pPocket); }
		void			_changeInstance(SkinSlotPocket*& pPocket, void* newInstance);

		void			_stateChanged(SkinSlotPocket*& pPocket, void* instance, State newState, State oldState);
		void			_valueChanged(SkinSlotPocket*& pPocket, void* instance, float newValue, float oldValue, float newValue2 = -1, float oldValue2 = -1);

		void 			_render(SkinSlotPocket* pPocket, GfxDevice* pDevice, const Rect& canvas, State state, float value = 1.f, float value2 = -1.f) const;

		inline Size		_minSize() const { return m_pSkin ? m_pSkin->minSize() : Size(); }
		inline Size		_preferredSize() const { return m_pSkin ? m_pSkin->preferredSize() : Size(); }

		inline Size		_contentPaddingSize() const { return m_pSkin ? m_pSkin->contentPaddingSize() : Size(); }
		inline Size		_sizeForContent(const Size& contentSize) const { return m_pSkin ? m_pSkin->sizeForContent(contentSize) : contentSize; }
		inline Coord	_contentOfs(State state) const { return m_pSkin ? m_pSkin->contentOfs(state) : Coord(); }
		inline Rect		_contentRect(const Rect& canvas, State state) const { return m_pSkin ? m_pSkin->contentRect(canvas, state) : canvas; }

		inline bool		_isOpaque() const { return m_pSkin ? m_pSkin->isOpaque() : false; }
		inline bool		_isOpaque(State state) const { return m_pSkin ? m_pSkin->isOpaque(state) : false; }
		inline bool		_isOpaque(const Rect& rect, const Size& canvasSize, State state) const
		{
			return m_pSkin ? m_pSkin->isOpaque(rect, canvasSize, state) : false;
		}

		inline bool		_markTest(const Coord& ofs, const Rect& canvas, State state, int opacityTreshold,
			float value = 1.f, float value2 = -1.f) const
		{
			return m_pSkin ? m_pSkin->markTest(ofs, canvas, state, opacityTreshold, value, value2) : false;
		}

		inline bool		_isContentShifting() { return m_pSkin ? m_pSkin->isContentShifting() : false; }

		//

		Object* _object() override { return m_pHolder->_object(); }
		const Object* _object() const override { return m_pHolder->_object(); }

		//

		bool	_update(SkinSlotPocket * pPocket, int msPassed);

		void	_initPocket(SkinSlotPocket*& pPocket, void * instance, State state);
		void	_releasePocket(SkinSlotPocket*& pPocket);

		//

		Skin_p	m_pSkin;
		Holder* m_pHolder;

	};



	class OCSkinSlotMI : public CSkinSlotMI
	{
	public:
		using CSkinSlotMI::_addInstance;
		using CSkinSlotMI::_removeInstance;
		using CSkinSlotMI::_stateChanged;
		using CSkinSlotMI::_valueChanged;
		using CSkinSlotMI::_render;
		using CSkinSlotMI::_minSize;
		using CSkinSlotMI::_preferredSize;
		using CSkinSlotMI::_contentPaddingSize;
		using CSkinSlotMI::_sizeForContent;
		using CSkinSlotMI::_contentOfs;
		using CSkinSlotMI::_contentRect;
		using CSkinSlotMI::_isOpaque;
		using CSkinSlotMI::_markTest;
		using CSkinSlotMI::_isContentShifting;
	};

	inline OCSkinSlotMI& OO(CSkinSlotMI& skin) { return reinterpret_cast<OCSkinSlotMI&>(skin); }
	inline const OCSkinSlotMI& OO(const CSkinSlotMI& skin) { return reinterpret_cast<const OCSkinSlotMI&>(skin); }



};

#endif //WG_CSKINSLOTMI_DOT_H