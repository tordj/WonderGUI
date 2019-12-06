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
#ifndef	WG_STRONGCOMPONENTPTR_DOT_H
#define	WG_STRONGCOMPONENTPTR_DOT_H
#pragma once

#include <wg_component.h>
#include <wg_object.h>

namespace wg
{

	template<class Cls> class StrongComponentPtr
	{
		template<class T> friend class StrongComponentPtr;

	public:

	//.____ Creation ___________________________________________________________

		StrongComponentPtr(Cls * const _p=0)
		{
			m_pComponent = _p;
			if( _p )
				static_cast<Component*>(_p)->_object()->_incRefCount();
		};

		StrongComponentPtr(const StrongComponentPtr<Cls>& _p)
		{
			m_pComponent = _p.m_pComponent;
			if (_p)
				static_cast<Component*>(_p)->_object()->_incRefCount();
		};


		template<typename _Tp1> StrongComponentPtr( const StrongComponentPtr<_Tp1>& _p)
		{
			m_pComponent = _p.m_pComponent;
			if (_p)
				static_cast<Component*>(_p)->_object()->_incRefCount();
		}

		~StrongComponentPtr()
		{
			if( m_pComponent )
				static_cast<Component*>(m_pComponent)->_object()->_decRefCount();
		}

		//.____ Operators ______________________________________________________

		StrongComponentPtr<Cls>& operator=(Cls * const _p)
		{
			if (m_pComponent != _p)
			{
				if (m_pComponent)
					static_cast<Component*>(m_pComponent)->_object()->_decRefCount();

				m_pComponent = _p;
				if (m_pComponent)
					static_cast<Component*>(m_pComponent)->_object()->_incRefCount();
			}
			return *this;
		}

		StrongComponentPtr<Cls>& operator=( const StrongComponentPtr<Cls>& _p)
		{
			if( m_pComponent != _p.m_pComponent )
			{
				if( m_pComponent )
					static_cast<Component*>(m_pComponent)->_object()->_decRefCount();

				m_pComponent = _p.m_pComponent;
				if (m_pComponent)
					static_cast<Component*>(m_pComponent)->_object()->_incRefCount();
			}
			return *this;
		}

		template<typename _Tp1> StrongComponentPtr<Cls>& operator=( const StrongComponentPtr<_Tp1>& _p)
		{
			if (m_pComponent != _p.m_pComponent)
			{
				if (m_pComponent)
					static_cast<Component*>(m_pComponent)->_object()->_decRefCount();

				m_pComponent = _p.m_pComponent;
				if (m_pComponent)
					static_cast<Component*>(m_pComponent)->_object()->_incRefCount();
			}
			return *this;
		}

		template<typename _Tp1> StrongComponentPtr<Cls>& operator=( _Tp1 * _p)
		{
			if (m_pComponent != _p)
			{
				if (m_pComponent)
					static_cast<Component*>(m_pComponent)->_object()->_decRefCount();

				m_pComponent = _p;
				if (m_pComponent)
					static_cast<Component*>(m_pComponent)->_object()->_incRefCount();
			}
			return *this;
		}

		operator  Cls*() const { return m_pComponent; }

		inline Cls & operator*() const { return * this->m_pComponent; }
		inline Cls * operator->() const{ return this->m_pComponent; }

		template<typename _Tp1> bool operator==( const StrongComponentPtr<_Tp1>& _p) const { return this->m_pComponent == _p.m_pComponent; }
		template<typename _Tp1> bool operator!=( const StrongComponentPtr<_Tp1>& _p) const { return this->m_pComponent != _p.m_pComponent; }
		template<typename _Tp1> bool operator<(const StrongComponentPtr<_Tp1>& other) const { return m_pComponent < other.m_pComponent; }
		template<typename _Tp1> bool operator>(const StrongComponentPtr<_Tp1>& other) const { return m_pComponent > other.m_pComponent; }
		template<typename _Tp1> bool operator<=(const StrongComponentPtr<_Tp1>& other) const { return m_pComponent <= other.m_pComponent; }
		template<typename _Tp1> bool operator>=(const StrongComponentPtr<_Tp1>& other) const { return m_pComponent >= other.m_pComponent; }

		inline operator bool() const { return m_pComponent != 0; }

		//.____ Misc ___________________________________________________________

		inline Cls* rawPtr() const { return m_pComponent; }


protected:
		Cls *	m_pComponent;
	};




} // namespace wg
#endif //WG_STRONGCOMPONENTPTR_DOT_H

