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
#ifndef	WG_WEAKINTERFACEPTR_DOT_H
#define	WG_WEAKINTERFACEPTR_DOT_H
#pragma once

#include <wg_object.h>

namespace wg 
{
	
	template<class Cls> class WeakInterfacePtr		/** @private */
	{
	public:
		WeakInterfacePtr(Cls * const _p=0)
		{
			m_pInterface = _p;
			if (_p)
				m_pHub = WeakPtrHub::getHub(_p->_object());
			else
				m_pHub = nullptr;
		};

		WeakInterfacePtr(const WeakInterfacePtr<Cls>& _p)
		{
			m_pInterface = _p.m_pInterface;
			m_pHub = _p.m_pHub;
			if( m_pHub )
				m_pHub->refCnt++;			
		};

	
		template<typename _Tp1> WeakInterfacePtr( const WeakInterfacePtr<_Tp1>& _p)
	    {
			m_pInterface = _p.m_pInterface;
			m_pHub = _p.m_pHub;
			if( m_pHub )
				m_pHub->refCnt++;			
	    }

		~WeakInterfacePtr()
		{
			WeakPtrHub::releaseHub(m_pHub);
		}

		WeakInterfacePtr<Cls>& operator=(Cls * const _p)
		{
			m_pInterface = _p;

			if (m_pHub && (!_p || m_pHub->pObj != _p->_object()))
			{
				WeakPtrHub::releaseHub(m_pHub);
				m_pHub = nullptr;
			}

			if (_p && !m_pHub)
				m_pHub = WeakPtrHub::getHub(_p->_object());

			return *this;
		}

	    WeakInterfacePtr<Cls>& operator=( const WeakInterfacePtr<Cls>& _p)
		{
			m_pInterface = _p.m_pInterface;
			if( m_pHub != _p.m_pHub )
			{
				WeakPtrHub::releaseHub(m_pHub);				
				m_pHub = _p.m_pHub;
				if( m_pHub )
					m_pHub->refCnt++;			
			}
			return *this;
		}

	    template<typename _Tp1> WeakInterfacePtr<Cls>& operator=( const WeakInterfacePtr<_Tp1>& _p)
		{
			m_pInterface = _p.m_pInterface;
			if( m_pHub != _p.m_pHub )
			{
				WeakPtrHub::releaseHub(m_pHub);				
				m_pHub = _p.m_pHub;
				if( m_pHub )
					m_pHub->refCnt++;			
			}
			return *this;
		}

	    template<typename _Tp1> WeakInterfacePtr<Cls>& operator=( _Tp1 * _p)
		{
			m_pInterface = _p;

			if (m_pHub && (!_p || m_pHub->pObj != _p->_object()))
			{
				WeakPtrHub::releaseHub(m_pHub);
				m_pHub = nullptr;
			}

			if( _p && !m_pHub )
				m_pHub = WeakPtrHub::getHub(_p->_object());

			return *this;
		}

		inline Cls * rawPtr() const	{ return m_pInterface; }
	
		operator  Cls*() const { return m_pInterface; }

		inline Cls & operator*() const { return * m_pInterface; }
		inline Cls * operator->() const{ return m_pInterface; }

		template<typename _Tp1> bool operator==(const StrongInterfacePtr<_Tp1>& _p) const { return this->m_pInterface == _p.m_pInterface; }
		template<typename _Tp1> bool operator!=(const StrongInterfacePtr<_Tp1>& _p) const { return this->m_pInterface != _p.m_pInterface; }
		template<typename _Tp1> bool operator<(const StrongInterfacePtr<_Tp1>& other) const { return m_pInterface < other.m_pInterface; }
		template<typename _Tp1> bool operator>(const StrongInterfacePtr<_Tp1>& other) const { return m_pInterface > other.m_pInterface; }
		template<typename _Tp1> bool operator<=(const StrongInterfacePtr<_Tp1>& other) const { return m_pInterface <= other.m_pInterface; }
		template<typename _Tp1> bool operator>=(const StrongInterfacePtr<_Tp1>& other) const { return m_pInterface >= other.m_pInterface; }

		inline operator bool() const { return m_pInterface != 0; }

		WeakPtrHub *	m_pHub;
		Cls *			m_pInterface;

	};

	
	

} // namespace wg
#endif //WG_WEAKINTERFACEPTR_DOT_H

