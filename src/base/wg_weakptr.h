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
#ifndef	WG_WEAKPTR_DOT_H
#define	WG_WEAKPTR_DOT_H
#pragma once

#include <wg_object.h>

namespace wg 
{
	
	template<class Cls> class WeakPtr		/** @private */
	{
	public:
		WeakPtr(Cls * const _p=0)
		{
			m_pHub = WeakPtrHub::getHub(_p);
		};

		WeakPtr(const WeakPtr<Cls>& _p)
		{
			m_pHub = _p.m_pHub;
			if( m_pHub )
				m_pHub->refCnt++;			
		};

	
		template<typename _Tp1> WeakPtr( const WeakPtr<_Tp1>& _p)
	    {
			Cls * p = static_cast<Cls*>((_Tp1*) nullptr);			// Force compiler error if types are incompatible. 
			m_pHub = _p.m_pHub;
			if( m_pHub )
				m_pHub->refCnt++;			
	    }

		~WeakPtr()
		{
			WeakPtrHub::releaseHub(m_pHub);
		}

		WeakPtr<Cls>& operator=(Cls * const _p)
		{
			if (!m_pHub || m_pHub->pObj != _p)
			{
				WeakPtrHub::releaseHub(m_pHub);
				m_pHub = WeakPtrHub::getHub(_p);
			}
			return *this;
		}

		WeakPtr<Cls>& operator=( const WeakPtr<Cls>& _p)
		{
			if( m_pHub != _p.m_pHub )
			{
				WeakPtrHub::releaseHub(m_pHub);				
				m_pHub = _p.m_pHub;
				if( m_pHub )
					m_pHub->refCnt++;			
			}
			return *this;
		}

	    template<typename _Tp1> WeakPtr<Cls>& operator=( const WeakPtr<_Tp1>& _p)
		{
			Cls * p = static_cast<Cls*>((_Tp1*) nullptr);			// Force compiler error if types are incompatible. 
			if( m_pHub != _p.m_pHub )
			{
				WeakPtrHub::releaseHub(m_pHub);				
				m_pHub = _p.m_pHub;
				if( m_pHub )
					m_pHub->refCnt++;			
			}
			return *this;
		}

	    template<typename _Tp1> WeakPtr<Cls>& operator=( _Tp1 * _p)
		{
			if( !m_pHub || m_pHub->pObj != _p )
			{
				WeakPtrHub::releaseHub(m_pHub);				
				m_pHub = WeakPtrHub::getHub(_p);
			}
			return *this;
		}

		inline Cls * rawPtr() const
		{
			if( this->m_pHub && this->m_pHub->pObj )
				return static_cast<Cls*>(this->m_pHub->pObj);
			else
				return nullptr;
		}
	
		operator  Cls*() const { return rawPtr(); }

		inline Cls & operator*() const { return * rawPtr(); }
		inline Cls * operator->() const{ return rawPtr(); }


		//TODO: Fix so that we get right value if both are null-pointers, but have different hubs.

		template<typename _Tp1> bool operator==( const WeakPtr<_Tp1>& _p) const
	    { 
			Cls * p = static_cast<Cls*>((_Tp1*) nullptr);			// Force compiler error if types are incompatible. 
	    	return this->m_pHub == _p.m_pHub;
	    }

		template<typename _Tp1> bool operator!=( const WeakPtr<_Tp1>& _p) const
	    { 
			Cls * p = static_cast<Cls*>((_Tp1*) nullptr);			// Force compiler error if types are incompatible. 
	    	return this->m_pHub != _p.m_pHub;
	    }

		template<typename _Tp1> bool operator<(const WeakPtr<_Tp1>& other) const { return m_pHub < other.m_pHub; }
		template<typename _Tp1> bool operator>(const WeakPtr<_Tp1>& other) const { return m_pHub > other.m_pHub; }
		template<typename _Tp1> bool operator<=(const WeakPtr<_Tp1>& other) const { return m_pHub <= other.m_pHub; }
		template<typename _Tp1> bool operator>=(const WeakPtr<_Tp1>& other) const { return m_pHub >= other.m_pHub; }

		inline operator bool() const { return (m_pHub != 0 && m_pHub->pObj != 0); }

		WeakPtrHub * m_pHub;
	};

	
	

} // namespace wg
#endif //WG_WEAKPTR_DOT_H

