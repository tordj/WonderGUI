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
#ifndef	WG_INTERFACEPTR_DOT_H
#define	WG_INTERFACEPTR_DOT_H
#pragma once


namespace wg 
{
	
	template<class Cls> class StrongInterfacePtr		/** @private */
	{
	public:
		StrongInterfacePtr(Cls * const _p=0)
		{
			m_pInterface = _p;
			if( _p )
				static_cast<Interface*>(_p)->_object()->_incRefCount();
		};

		StrongInterfacePtr(const StrongInterfacePtr<Cls>& _p)
		{
			m_pInterface = _p.m_pInterface;
			if (_p)
				static_cast<Interface*>(_p)->_object()->_incRefCount();
		};

	
		template<typename _Tp1> StrongInterfacePtr( const StrongInterfacePtr<_Tp1>& _p)
	    { 
			m_pInterface = _p.m_pInterface;
			if (_p)
				static_cast<Interface*>(_p)->_object()->_incRefCount();
	    }

		~StrongInterfacePtr()
		{
			if( m_pInterface )
				static_cast<Interface*>(m_pInterface)->_object()->_decRefCount();
		}

	    StrongInterfacePtr<Cls>& operator=( const StrongInterfacePtr<Cls>& _p)
		{
			if( m_pInterface != _p.m_pInterface )
			{
				if( m_pInterface )
					static_cast<Interface*>(m_pInterface)->_object()->_decRefCount();
				
				m_pInterface = _p.m_pInterface;
				if (m_pInterface)
					static_cast<Interface*>(m_pInterface)->_object()->_incRefCount();
			}
			return *this;
		}

	    template<typename _Tp1> StrongInterfacePtr<Cls>& operator=( const StrongInterfacePtr<_Tp1>& _p)
		{
			if (m_pInterface != _p.m_pInterface)
			{
				if (m_pInterface)
					static_cast<Interface*>(m_pInterface)->_object()->_decRefCount();

				m_pInterface = _p.m_pInterface;
				if (m_pInterface)
					static_cast<Interface*>(m_pInterface)->_object()->_incRefCount();
			}
			return *this;
		}

	    template<typename _Tp1> StrongInterfacePtr<Cls>& operator=( _Tp1 * _p)
		{
			if (m_pInterface != _p)
			{
				if (m_pInterface)
					static_cast<Interface*>(m_pInterface)->_object()->_decRefCount();

				m_pInterface = _p;
				if (m_pInterface)
					static_cast<Interface*>(m_pInterface)->_object()->_incRefCount();
			}
			return *this;
		}


		inline Cls* rawPtr() const { return m_pInterface; }
	
		operator  Cls*() const { return m_pInterface; }

		inline Cls & operator*() const { return * this->m_pInterface; }
		inline Cls * operator->() const{ return this->m_pInterface; }

		template<typename _Tp1> bool operator==( const StrongInterfacePtr<_Tp1>& _p) const { return this->m_pInterface == _p.m_pInterface; }
		template<typename _Tp1> bool operator!=( const StrongInterfacePtr<_Tp1>& _p) const { return this->m_pInterface != _p.m_pInterface; }
		template<typename _Tp1> bool operator<(const StrongInterfacePtr<_Tp1>& other) const { return m_pInterface < other.m_pInterface; }
		template<typename _Tp1> bool operator>(const StrongInterfacePtr<_Tp1>& other) const { return m_pInterface > other.m_pInterface; }
		template<typename _Tp1> bool operator<=(const StrongInterfacePtr<_Tp1>& other) const { return m_pInterface <= other.m_pInterface; }
		template<typename _Tp1> bool operator>=(const StrongInterfacePtr<_Tp1>& other) const { return m_pInterface >= other.m_pInterface; }

		inline operator bool() const { return m_pInterface != 0; }


		Cls *	m_pInterface;
	};

	
	

} // namespace wg
#endif //WG_INTERFACEPTR_DOT_H

