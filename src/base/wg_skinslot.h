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

#ifndef	WG_SKINSLOT_DOT_H
#define	WG_SKINSLOT_DOT_H
#pragma once

#include <wg_skin.h>

namespace wg
{

	class SkinSlot
	{
	public:

		class Holder
		{
		public:
			virtual void _skinRequestRender(SkinSlot* pSlot) = 0;
			virtual void _skinRequestRender(SkinSlot* pSlot, const Rect& rect ) = 0;

			virtual Object* _object() = 0;
		};

		SkinSlot(Holder* pHolder) : m_pHolder(pHolder) {}
		~SkinSlot() { if (m_pSkin) m_pSkin->_removeSlot(this); }


		// Operators for the holder

		inline Skin* operator->() const { return m_pSkin; }
		inline operator Skin*() const { return m_pSkin; }
		inline operator Skin_p() const { return m_pSkin; }

		inline SkinSlot& operator=(Skin* pSkin)
		{
			if (m_pSkin)
				m_pSkin->_removeSlot(this);

			m_pSkin = pSkin;

			if (m_pSkin)
				m_pSkin->_addSlot(this);

			return *this;
		}

		// Methods for the skin to call.

		inline void requestRender() { m_pHolder->_skinRequestRender( this ); }
		inline void requestRender( const Rect& rect) { m_pHolder->_skinRequestRender( this, rect); }
		inline Object* object() { return m_pHolder->_object(); }
	protected:
		Skin_p	m_pSkin;
		Holder* m_pHolder;

	};






}

;

#endif //WG_SKINSLOT_DOT_H