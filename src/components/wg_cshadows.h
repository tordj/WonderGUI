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

#ifndef	WG_CSHADOWS_DOT_H
#define	WG_CSHADOWS_DOT_H
#pragma once

#include <wg_component.h>
#include <wg_pointers.h>
#include <wg_widget.h>
#include <wg_skin.h>
#include <wg_shadow.h>

#include <vector>

namespace wg
{
	class CShadows;
	typedef	StrongComponentPtr<CShadows>	CShadows_p;
	typedef	WeakComponentPtr<CShadows>		CShadows_wp;

	//____ ShadowHolder ___________________________________________________

	class ShadowHolder /** @private */
	{
	public:
		virtual Object * _object() = 0;

		virtual void	_didAddShadows(int nb) = 0;
		virtual void	_willRemoveShadows(int ofs, int nb) = 0;
		virtual void	_setShadowTint(uint8_t alpha) = 0;
		virtual uint8_t _shadowTint() const = 0;


		inline void		_setShadowGeo( Shadow * pShadow, const RectI& geo ) { pShadow->m_geo = geo; }
		inline RectI	_shadowGeo( Shadow * pShadow ) { return pShadow->m_geo; }
	};

	//____ CShadows ______________________________________________________________

	class CShadows : public Component
	{
	public:

		using		iterator = std::vector<Shadow>::iterator;
		using		const_iterator = std::vector<Shadow>::const_iterator;

		/** @private */
		CShadows(ShadowHolder * pHolder, std::vector<Shadow> * pVector) : m_pHolder(pHolder), m_pShadows(pVector) {}

		//.____ Operators __________________________________________

		const Shadow& operator[](int index) const { return m_pShadows->operator[](index); }
		inline CShadows& operator<<(std::tuple<Widget*, Skin*> shadow) { add(std::get<0>(shadow), std::get<1>(shadow)); return *this; }
		inline CShadows& operator<<(std::initializer_list <std::tuple<Widget*, Skin*>> shadows) { add(shadows); return *this; }

		//.____ Appearance ________________________________________________________

		void		setTint(uint8_t alpha) { m_pHolder->_setShadowTint(alpha); }
		uint8_t		tint() const { return m_pHolder->_shadowTint(); }

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

		inline CShadows_p ptr() { return CShadows_p(this); }

		inline iterator	begin() { return m_pShadows->begin(); }
		inline iterator	end() { return m_pShadows->end(); }

		inline const_iterator	begin() const { return m_pShadows->begin(); }
		inline const_iterator	end() const { return m_pShadows->end(); }

	private:

		virtual Object * 	_object() const override { return m_pHolder->_object(); }

		ShadowHolder *			m_pHolder;
		std::vector<Shadow>*	m_pShadows;

	};


} // namespace wg
#endif //WG_CSHADOWS_DOT_H
