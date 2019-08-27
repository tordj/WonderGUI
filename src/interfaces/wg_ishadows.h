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

#ifndef	WG_ISHADOWS_DOT_H
#define	WG_ISHADOWS_DOT_H
#pragma once

#include <wg_interface.h>
#include <wg_pointers.h>
#include <wg_widget.h>
#include <wg_skin.h>
#include <wg_shadow.h>

#include <vector>

namespace wg
{
	class IShadows;
	typedef	StrongInterfacePtr<IShadows>	IShadows_p;
	typedef	WeakInterfacePtr<IShadows>		IShadows_wp;

	//____ ShadowHolder ___________________________________________________
	
	class ShadowHolder /** @private */
	{
	public:
		virtual Object * _object() = 0;

		virtual void	_didAddShadows(int nb) = 0;
		virtual void	_willRemoveShadows(int ofs, int nb) = 0;
		
		inline void		_setShadowGeo( Shadow * pShadow, const RectI& geo ) { pShadow->m_geo = geo; }
		inline RectI	_shadowGeo( Shadow * pShadow ) { return pShadow->m_geo; }
	};

	//____ IShadows ______________________________________________________________

	class IShadows : public Interface
	{
	public:

		using		iterator = std::vector<Shadow>::iterator;
		using		const_iterator = std::vector<Shadow>::const_iterator;

		/** @private */
		IShadows(ShadowHolder * pHolder, std::vector<Shadow> * pVector) : m_pHolder(pHolder), m_pShadows(pVector) {}

		//.____ Operators __________________________________________

		const Shadow& operator[](int index) const { return m_pShadows->operator[](index); }
		inline IShadows& operator<<(std::tuple<Widget*, Skin*> shadow) { add(std::get<0>(shadow), std::get<1>(shadow)); return *this; }
		inline IShadows& operator<<(std::initializer_list <std::tuple<Widget*, Skin*>> shadows) { add(shadows); return *this; }

		//.____ Content _______________________________________________________

		int			size() const { return (int) m_pShadows->size(); }
		int			isEmpty() const { return m_pShadows->empty(); }

		void		clear();
		bool		add(Widget * pWidget, Skin * pShadow);
		int			add(std::initializer_list <std::tuple<Widget*, Skin*>> shadows);

		void		remove(int index);
		iterator	remove(const_iterator pos);
		void		remove(int index, int amount);
		iterator	remove(const_iterator beg, const_iterator end);

		//.____ Misc _______________________________________________________

		inline IShadows_p ptr() { return IShadows_p(this); }

		inline iterator	begin() { return m_pShadows->begin(); }
		inline iterator	end() { return m_pShadows->end(); }

		inline const_iterator	begin() const { return m_pShadows->begin(); }
		inline const_iterator	end() const { return m_pShadows->end(); }

	private:

		virtual Object * 	_object() const { return m_pHolder->_object(); }

		ShadowHolder *			m_pHolder;
		std::vector<Shadow>*	m_pShadows;

	};


} // namespace wg
#endif //WG_IDYNAMICCHILDREN_DOT_H
