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
#ifndef	WG_WeakComponentPtr_DOT_H
#define	WG_WeakComponentPtr_DOT_H
#pragma once

#include <wg_object.h>

namespace wg
{

	template<class Cls> class WeakComponentPtr		/** @private */
	{
	public:
		WeakComponentPtr(Cls * const _p=0)
		{
			m_pComponent = _p;
			if (_p)
				m_pHub = WeakPtrHub::getHub(_p->_object());
			else
				m_pHub = nullptr;
		};

		WeakComponentPtr(const WeakComponentPtr<Cls>& _p)
		{
			m_pComponent = _p.m_pComponent;
			m_pHub = _p.m_pHub;
			if( m_pHub )
				m_pHub->refCnt++;
		};


		template<typename _Tp1> WeakComponentPtr( const WeakComponentPtr<_Tp1>& _p)
		{
			m_pComponent = _p.m_pComponent;
			m_pHub = _p.m_pHub;
			if( m_pHub )
				m_pHub->refCnt++;
		}

		~WeakComponentPtr()
		{
			WeakPtrHub::releaseHub(m_pHub);
		}

		WeakComponentPtr<Cls>& operator=(Cls * const _p)
		{
			m_pComponent = _p;

			if (m_pHub && (!_p || m_pHub->pObj != _p->_object()))
			{
				WeakPtrHub::releaseHub(m_pHub);
				m_pHub = nullptr;
			}

			if (_p && !m_pHub)
				m_pHub = WeakPtrHub::getHub(_p->_object());

			return *this;
		}

		WeakComponentPtr<Cls>& operator=( const WeakComponentPtr<Cls>& _p)
		{
			m_pComponent = _p.m_pComponent;
			if( m_pHub != _p.m_pHub )
			{
				WeakPtrHub::releaseHub(m_pHub);
				m_pHub = _p.m_pHub;
				if( m_pHub )
					m_pHub->refCnt++;
			}
			return *this;
		}

		template<typename _Tp1> WeakComponentPtr<Cls>& operator=( const WeakComponentPtr<_Tp1>& _p)
		{
			m_pComponent = _p.m_pComponent;
			if( m_pHub != _p.m_pHub )
			{
				WeakPtrHub::releaseHub(m_pHub);
				m_pHub = _p.m_pHub;
				if( m_pHub )
					m_pHub->refCnt++;
			}
			return *this;
		}

		template<typename _Tp1> WeakComponentPtr<Cls>& operator=( _Tp1 * _p)
		{
			m_pComponent = _p;

			if (m_pHub && (!_p || m_pHub->pObj != _p->_object()))
			{
				WeakPtrHub::releaseHub(m_pHub);
				m_pHub = nullptr;
			}

			if( _p && !m_pHub )
				m_pHub = WeakPtrHub::getHub(_p->_object());

			return *this;
		}

		inline Cls * rawPtr() const	{ return m_pComponent; }

		operator  Cls*() const { return m_pComponent; }

		inline Cls & operator*() const { return * m_pComponent; }
		inline Cls * operator->() const{ return m_pComponent; }

		template<typename _Tp1> bool operator==(const StrongComponentPtr<_Tp1>& _p) const { return this->m_pComponent == _p.m_pComponent; }
		template<typename _Tp1> bool operator!=(const StrongComponentPtr<_Tp1>& _p) const { return this->m_pComponent != _p.m_pComponent; }
		template<typename _Tp1> bool operator<(const StrongComponentPtr<_Tp1>& other) const { return m_pComponent < other.m_pComponent; }
		template<typename _Tp1> bool operator>(const StrongComponentPtr<_Tp1>& other) const { return m_pComponent > other.m_pComponent; }
		template<typename _Tp1> bool operator<=(const StrongComponentPtr<_Tp1>& other) const { return m_pComponent <= other.m_pComponent; }
		template<typename _Tp1> bool operator>=(const StrongComponentPtr<_Tp1>& other) const { return m_pComponent >= other.m_pComponent; }

		inline operator bool() const { return m_pComponent != 0; }

		WeakPtrHub *	m_pHub;
		Cls *			m_pComponent;
	};




} // namespace wg
#endif //WG_WeakComponentPtr_DOT_H

