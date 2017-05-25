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
#ifndef	WG_POINTERS_DOT_H
#define	WG_POINTERS_DOT_H
#pragma once

#include <wg_mempool.h>
#include <wg_strongptr.h>
#include <wg_weakptr.h>
#include <wg_interfaceptr.h>
#include <wg_object.h>


namespace wg 
{
	class Interface;
	
	//____ Interface_wp ______________________________________________________________
	
	class Interface_wp
	{
	public:
		Interface_wp() { m_pHub = 0; m_pInterface = 0; }
		Interface_wp( Object * pObj, Interface * pInterface );
		Interface_wp( Interface * pInterface );
	
		Interface_wp(const Interface_wp& r)
		{
			m_pInterface = r.m_pInterface;
			m_pHub = r.m_pHub;
			if( m_pHub )
				m_pHub->refCnt++;
		}
	
		~Interface_wp();
	
	
	    inline Interface_wp& operator=( Interface_wp const & r)
		{
			copy( r );
			return *this;
		}
	
		inline Interface& operator*() const { return * rawPtr(); }
		inline Interface * operator->() const { return rawPtr(); }
	
		inline bool operator==(const Interface_wp& other) const { return m_pInterface == other.m_pInterface; }
		inline bool operator!=(const Interface_wp& other) const { return m_pInterface != other.m_pInterface; }
		inline bool operator<(const Interface_wp& other) const { return m_pInterface < other.m_pInterface ? true : false; }
		inline bool operator>(const Interface_wp& other) const { return m_pInterface > other.m_pInterface ? true : false; }
		inline bool operator<=(const Interface_wp& other) const { return m_pInterface <= other.m_pInterface ? true : false; }
		inline bool operator>=(const Interface_wp& other) const { return m_pInterface >= other.m_pInterface ? true : false; }
	
		inline operator bool() const { return (m_pHub != 0 && m_pHub->pObj != 0); }
	
		inline Interface * rawPtr() const
		{
			if( m_pHub && m_pHub->pObj )
				return m_pInterface;
			else
				return 0;
		}
	
		void copy( Interface_wp const & r );
	
		WeakPtrHub *	m_pHub;
		Interface *	m_pInterface;
	
	};
	
	//____ WeakInterfacePtr _________________________________________________________
	
	template<class T,class P> class WeakInterfacePtr : public P		/** @private */
	{
	public:
		WeakInterfacePtr(Object* pObj, T* pInterface=0) : P( pObj, pInterface ) {};
		WeakInterfacePtr(const WeakInterfacePtr<T,P>& r) : P( r.rawPtr(), r.m_pInterface ) {};
		~WeakInterfacePtr() {};
	
		inline T & operator*() const { return * rawPtr(); }
		inline T * operator->() const{ return rawPtr(); }
	
		inline bool operator==(const WeakInterfacePtr<T,P>& other) const { return this->m_pInterface == other.m_pInterface; }
		inline bool operator!=(const WeakInterfacePtr<T,P>& other) const { return this->m_pInterface != other.m_pInterface; }
	
		inline T * rawPtr() const
		{
			if( this->m_pHub && this->m_pHub->pObj )
				return static_cast<T*>(this->m_pInterface);
			else
				return reinterpret_cast<T*>(0);
		}
	};
	
	
	

} // namespace wg
#endif //WG_POINTERS_DOT_H

