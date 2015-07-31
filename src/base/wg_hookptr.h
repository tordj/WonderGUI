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

#ifndef WG_HOOKPTR_DOT_H

namespace wg 
{
	#define WG_HOOKPTR_DOT_H
	
	class Hook;
	
	class Hook_pHub
	{
	public:
		int				refCnt;
		Hook *		pObj;
	};
	
	class Hook_p
	{
	public:
		Hook_p() : m_pHub(0) {}
		Hook_p( Hook * pObj );
		Hook_p(const Hook_p& r)
		{
			m_pHub = r.m_pHub;
			if( m_pHub )
				m_pHub->refCnt++;
		}
	
		~Hook_p();
	
	    inline Hook_p& operator=( Hook_p const & r)
		{
			copy( r );
			return *this;
		}
	
		inline Hook& operator*() const { return * rawPtr(); }
		inline Hook * operator->() const { return rawPtr(); }
	
		//TODO: Fix so that we get right value if both are null-pointers, but have different hubs.
		inline bool operator==(const Hook_p& other) const { return m_pHub == other.m_pHub; }
		inline bool operator!=(const Hook_p& other) const { return m_pHub != other.m_pHub; }
		inline bool operator<(const Hook_p& other) const { return m_pHub < other.m_pHub ? true : false; }
		inline bool operator>(const Hook_p& other) const { return m_pHub > other.m_pHub ? true : false; }
		inline bool operator<=(const Hook_p& other) const { return m_pHub <= other.m_pHub ? true : false; }
		inline bool operator>=(const Hook_p& other) const { return m_pHub >= other.m_pHub ? true : false; }
	
		inline operator bool() const { return (m_pHub != 0 && m_pHub->pObj != 0); }
	
		inline Hook * rawPtr() const
		{
			if( m_pHub )
				return m_pHub->pObj;
			else
				return 0;
		}
	
		void copy( Hook_p const & r );
	
	
	
	protected:
		Hook_pHub *		m_pHub;
	
	};
	
	//____ HookTypePtr _________________________________________________________
	
	template<class T,class P> class HookTypePtr : public P
	{
	public:
		HookTypePtr(T* p=0) : P( p ) {};
		HookTypePtr(const HookTypePtr<T,P>& r) : P( r.rawPtr() ) {};
	//	WgWeakPtr(const WgStrongPtr<T,P>& r) : P( r.rawPtr() ) {};
		~HookTypePtr() {};
	
		inline T & operator*() const { return * rawPtr(); }
		inline T * operator->() const{ return rawPtr(); }
	
		inline bool operator==(const HookTypePtr<T,P>& other) const { return this->m_pHub == other.m_pHub; }
		inline bool operator!=(const HookTypePtr<T,P>& other) const { return this->m_pHub != other.m_pHub; }
	
	//	inline operator bool() const { return (this->m_pObj != 0); }
	
		inline T * rawPtr() const
		{
			if( this->m_pHub && this->m_pHub->pObj )
				return static_cast<T*>(this->m_pHub->pObj);
			else
				return reinterpret_cast<T*>(0);
		}
	};
	
	
	

} // namespace wg
#endif //WG_HOOKPTR_DOT_H
