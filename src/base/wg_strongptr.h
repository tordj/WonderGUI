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
#ifndef	WG_STRONGPTR_DOT_H
#define	WG_STRONGPTR_DOT_H
#pragma once


namespace wg 
{
	
	template<class Cls> class StrongPtr		/** @private */
	{
		template<class T> friend class StrongPtr;
		
		
	public:

		//.____ Creation ___________________________________________________________


		StrongPtr(Cls * const _p=0)
		{
	    	m_pObj = _p;
			if( _p )
				_p->_incRefCount();			
		};

		StrongPtr(const StrongPtr<Cls>& _p)
		{
	    	m_pObj = _p.m_pObj;
			if( _p.m_pObj )
				_p->_incRefCount();			
		};

	
		template<typename _Tp1> StrongPtr( const StrongPtr<_Tp1>& _p)
	    { 
	    	m_pObj = _p.m_pObj;
			if( _p.m_pObj )
				_p.m_pObj->_incRefCount();
	    }

		~StrongPtr()
		{
			if( m_pObj )
				m_pObj->_decRefCount();
		}
		
		//.____ Operators ______________________________________________________
				
		StrongPtr<Cls>& operator=(Cls * const _p)
		{
			if (m_pObj != _p)
			{
				if (m_pObj)
					m_pObj->_decRefCount();

				m_pObj = _p;
				if (m_pObj)
					m_pObj->_incRefCount();
			}
			return *this;
		}

		StrongPtr<Cls>& operator=( const StrongPtr<Cls>& _p)
		{
			if( m_pObj != _p.m_pObj )
			{
				if( m_pObj )
					m_pObj->_decRefCount();
	
				m_pObj = _p.m_pObj;
				if( m_pObj )
					m_pObj->_incRefCount();
			}
			return *this;
		}

	    template<typename _Tp1> StrongPtr<Cls>& operator=( const StrongPtr<_Tp1>& _p)
		{
			if( m_pObj != _p.m_pObj )
			{
				if( m_pObj )
					m_pObj->_decRefCount();
	
				m_pObj = _p.m_pObj;
				if( m_pObj )
					m_pObj->_incRefCount();
			}
			return *this;
		}

	    template<typename _Tp1> StrongPtr<Cls>& operator=( _Tp1 * _p)
		{
			if( m_pObj != _p )
			{
				if( m_pObj )
					m_pObj->_decRefCount();
	
				m_pObj = _p;
				if( m_pObj )
					m_pObj->_incRefCount();
			}
			return *this;
		}

	
		operator  Cls*() const { return m_pObj; }

		inline Cls & operator*() const { return * this->m_pObj; }
		inline Cls * operator->() const{ return this->m_pObj; }

		template<typename _Tp1> bool operator==( const StrongPtr<_Tp1>& _p) const
	    { 
	    	return this->m_pObj == _p.m_pObj;
	    }

		template<typename _Tp1> bool operator!=( const StrongPtr<_Tp1>& _p) const
	    { 
	    	return this->m_pObj != _p.m_pObj;
	    }

		template<typename _Tp1> bool operator<(const StrongPtr<_Tp1>& other) const { return m_pObj < other.m_pObj; }
		template<typename _Tp1> bool operator>(const StrongPtr<_Tp1>& other) const { return m_pObj > other.m_pObj; }
		template<typename _Tp1> bool operator<=(const StrongPtr<_Tp1>& other) const { return m_pObj <= other.m_pObj; }
		template<typename _Tp1> bool operator>=(const StrongPtr<_Tp1>& other) const { return m_pObj >= other.m_pObj; }

		inline operator bool() const { return m_pObj != 0; }

		//.____ Misc ___________________________________________________________

		inline Cls* rawPtr() const { return m_pObj; }


protected:
		Cls *	m_pObj;
	};

	
	

} // namespace wg
#endif //WG_STRONGPTR_DOT_H

