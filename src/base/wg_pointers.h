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

#ifndef WG_MEMPOOL_DOT_H
#	include <wg_mempool.h>
#endif

#ifndef	WG_OBJECT_DOT_H
#	include <wg_object.h>
#endif

#ifndef WG_INTERFACE_DOT_H
#	include	<wg_interface.h>
#endif

class WgObject_wp;

//____ WgObject_p _____________________________________________________________

class WgObject_p
{
public:
	WgObject_p(WgObject* p=0)
	{
		m_pObj = p;
		if( p )
			p->_incRefCount();
	}

	WgObject_p(const WgObject_p& r)
	{
		m_pObj = r.m_pObj;
		if( m_pObj )
			m_pObj->_incRefCount();
	}

	WgObject_p( const WgObject_wp& r );

	~WgObject_p()
	{
		if( m_pObj )
			m_pObj->_decRefCount();
	}


    inline WgObject_p & operator=( WgObject_p const & r)
	{
		copy( r );
		return *this;
	}

	inline WgObject& operator*() const { return * m_pObj; }
	inline WgObject* operator->() const{ return m_pObj; }

	inline bool operator==(const WgObject_p& other) const { return m_pObj == other.m_pObj; }
	inline bool operator!=(const WgObject_p& other) const { return m_pObj != other.m_pObj; }

	inline operator bool() const { return (m_pObj != 0); }

	inline WgObject * rawPtr() const { return m_pObj; }

protected:
	void copy( WgObject_p const & r )
	{
		if( m_pObj != r.m_pObj )
		{
			if( m_pObj )
				m_pObj->_decRefCount();

			m_pObj = r.m_pObj;
			if( m_pObj )
				m_pObj->_incRefCount();
		}
	}


	WgObject * m_pObj;
};


//____ WgStrongPtr ________________________________________________________

template<class T,class P> class WgStrongPtr : public P
{
public:
	WgStrongPtr(T* p=0) : P( p ) {};
	WgStrongPtr(const WgStrongPtr<T,P>& r) : P( (T*) r.m_pObj ) {};
//	WgStrongPtr(const WgWeakPtr<T,P>& r) : P( (T*) r.rawPtr() ) {};
	~WgStrongPtr() {};

/*
    inline WgStrongPtr<T,P> & operator=( WgStrongPtr<T,P> const & r)
	{
		copy( r );
		return *this;
	}
*/
	inline T & operator*() const { return * (T*) this->m_pObj; }
	inline T * operator->() const{ return (T*) this->m_pObj; }

	inline bool operator==(const WgStrongPtr<T,P>& other) const { return this->m_pObj == other.m_pObj; }
	inline bool operator!=(const WgStrongPtr<T,P>& other) const { return this->m_pObj != other.m_pObj; }

//	inline operator bool() const { return (this->m_pObj != 0); }

	inline T * rawPtr() const { return (T*) this->m_pObj; }
};


//____ WgObject_wp ______________________________________________________________

class WgObject_wp
{
public:
	WgObject_wp() : m_pHub(0) {}
	WgObject_wp( WgObject * pObj );
	WgObject_wp(const WgObject_p& r);
	WgObject_wp(const WgObject_wp& r)
	{

		m_pHub = r.m_pHub;
		if( m_pHub )
			m_pHub->refCnt++;
	}

	~WgObject_wp();

    inline WgObject_wp& operator=( WgObject_wp const & r)
	{
		copy( r );
		return *this;
	}

	inline WgObject& operator*() const { return * rawPtr(); }
	inline WgObject * operator->() const { return rawPtr(); }

	//TODO: Fix so that we get right value if both are null-pointers, but have different hubs.
	inline bool operator==(const WgObject_wp& other) const { return m_pHub == other.m_pHub; }
	inline bool operator!=(const WgObject_wp& other) const { return m_pHub != other.m_pHub; }
	inline bool operator<(const WgObject_wp& other) const { return m_pHub < other.m_pHub ? true : false; }
	inline bool operator>(const WgObject_wp& other) const { return m_pHub > other.m_pHub ? true : false; }
	inline bool operator<=(const WgObject_wp& other) const { return m_pHub <= other.m_pHub ? true : false; }
	inline bool operator>=(const WgObject_wp& other) const { return m_pHub >= other.m_pHub ? true : false; }

	inline operator bool() const { return (m_pHub != 0 && m_pHub->pObj != 0); }

	inline WgObject * rawPtr() const
	{
		if( m_pHub )
			return m_pHub->pObj;
		else
			return 0;
	}

	void copy( WgObject_wp const & r );

	WgWeakPtrHub * m_pHub;

};

//____ WgWeakPtr _________________________________________________________

template<class T,class P> class WgWeakPtr : public P
{
public:
	WgWeakPtr() {}
	WgWeakPtr(T* p) : P( p ) {};
	WgWeakPtr(const WgWeakPtr<T,P>& r) : P( r.rawPtr() ) {};
//	WgWeakPtr(const WgStrongPtr<T,P>& r) : P( r.rawPtr() ) {};
	~WgWeakPtr() {};

	inline T & operator*() const { return * rawPtr(); }
	inline T * operator->() const{ return rawPtr(); }

	inline bool operator==(const WgWeakPtr<T,P>& other) const { return this->m_pHub == other.m_pHub; }
	inline bool operator!=(const WgWeakPtr<T,P>& other) const { return this->m_pHub != other.m_pHub; }

//	inline operator bool() const { return (this->m_pObj != 0); }

	inline T * rawPtr() const
	{
		if( this->m_pHub && this->m_pHub->pObj )
			return static_cast<T*>(this->m_pHub->pObj);
		else
			return reinterpret_cast<T*>(0);
	}
};


//____ WgInterface_p _____________________________________________________________

// m_pObj and m_pInterface must both be valid or null.

class WgInterface_p
{
public:

	WgInterface_p()
	{
		m_pObj = 0;
		m_pInterface = 0;
	}

	WgInterface_p(WgObject* pObj, WgInterface* pInterface )
	{
		m_pInterface = pInterface;
		m_pObj = pObj;
		if( pObj )
			pObj->_incRefCount();
	}

	WgInterface_p(const WgInterface_p& r)
	{
		m_pInterface = r.m_pInterface;
		m_pObj = r.m_pObj;
		if( m_pObj )
			m_pObj->_incRefCount();
	}

	~WgInterface_p()
	{
		if( m_pObj )
			m_pObj->_decRefCount();
	}


    inline WgInterface_p & operator=( WgInterface_p const & r)
	{
		copy( r );
		return *this;
	}

	inline WgInterface& operator*() const { return * m_pInterface; }
	inline WgInterface* operator->() const{ return m_pInterface; }

	inline bool operator==(const WgInterface_p& other) const { return m_pInterface == other.m_pInterface; }
	inline bool operator!=(const WgInterface_p& other) const { return m_pInterface != other.m_pInterface; }

	inline operator bool() const { return (m_pInterface != 0); }

	inline WgInterface * rawPtr() const { return m_pInterface; }
	inline WgObject * getRealObjectPtr() const { return m_pObj; }

protected:
	void copy( WgInterface_p const & r )
	{
		m_pInterface = r.m_pInterface;
		if( m_pObj != r.m_pObj )
		{
			if( m_pObj )
				m_pObj->_decRefCount();

			m_pObj = r.m_pObj;
			if( m_pObj )
				m_pObj->_incRefCount();
		}
	}


	WgObject *		m_pObj;
	WgInterface *	m_pInterface;
};

//____ WgIStrongPtr ________________________________________________________

template<class T,class P> class WgIStrongPtr : public P
{
public:
	WgIStrongPtr( int dummy = 0 ) : P( 0, 0 ) {};
	WgIStrongPtr(WgObject* pObj, T* pInterface) : P( pObj, pInterface ) {};
	WgIStrongPtr(const WgIStrongPtr<T,P>& r) : P( r.m_pObj, (T*) r.m_pInterface ) {};
	~WgIStrongPtr() {};

	inline T & operator*() const { return * (T*) this->m_pInterface; }
	inline T * operator->() const{ return (T*) this->m_pInterface; }

	inline bool operator==(const WgIStrongPtr<T,P>& other) const { return this->m_pInterface == other.m_pInterface; }
	inline bool operator!=(const WgIStrongPtr<T,P>& other) const { return this->m_pInterface != other.m_pInterface; }

	inline T * rawPtr() const { return (T*) this->m_pInterface; }
};


//____ WgInterface_wp ______________________________________________________________

class WgInterface_wp
{
public:
	WgInterface_wp() { m_pHub = 0; m_pInterface = 0; }
	WgInterface_wp( WgObject * pObj, WgInterface * pInterface );

	WgInterface_wp(const WgInterface_wp& r)
	{
		m_pInterface = r.m_pInterface;
		m_pHub = r.m_pHub;
		if( m_pHub )
			m_pHub->refCnt++;
	}

	~WgInterface_wp();


    inline WgInterface_wp& operator=( WgInterface_wp const & r)
	{
		copy( r );
		return *this;
	}

	inline WgInterface& operator*() const { return * rawPtr(); }
	inline WgInterface * operator->() const { return rawPtr(); }

	inline bool operator==(const WgInterface_wp& other) const { return m_pInterface == other.m_pInterface; }
	inline bool operator!=(const WgInterface_wp& other) const { return m_pInterface != other.m_pInterface; }
	inline bool operator<(const WgInterface_wp& other) const { return m_pInterface < other.m_pInterface ? true : false; }
	inline bool operator>(const WgInterface_wp& other) const { return m_pInterface > other.m_pInterface ? true : false; }
	inline bool operator<=(const WgInterface_wp& other) const { return m_pInterface <= other.m_pInterface ? true : false; }
	inline bool operator>=(const WgInterface_wp& other) const { return m_pInterface >= other.m_pInterface ? true : false; }

	inline operator bool() const { return (m_pHub != 0 && m_pHub->pObj != 0); }

	inline WgInterface * rawPtr() const
	{
		if( m_pHub && m_pHub->pObj )
			return m_pInterface;
		else
			return 0;
	}

	void copy( WgInterface_wp const & r );

	WgWeakPtrHub *	m_pHub;
	WgInterface *	m_pInterface;

};

//____ WgIWeakPtr _________________________________________________________

template<class T,class P> class WgIWeakPtr : public P
{
public:
	WgIWeakPtr(WgObject* pObj, T* pInterface=0) : P( pObj, pInterface ) {};
	WgIWeakPtr(const WgIWeakPtr<T,P>& r) : P( r.rawPtr(), r.m_pInterface ) {};
	~WgIWeakPtr() {};

	inline T & operator*() const { return * rawPtr(); }
	inline T * operator->() const{ return rawPtr(); }

	inline bool operator==(const WgIWeakPtr<T,P>& other) const { return this->m_pInterface == other.m_pInterface; }
	inline bool operator!=(const WgIWeakPtr<T,P>& other) const { return this->m_pInterface != other.m_pInterface; }

	inline T * rawPtr() const
	{
		if( this->m_pHub && this->m_pHub->pObj )
			return static_cast<T*>(this->m_pInterface);
		else
			return reinterpret_cast<T*>(0);
	}
};



#endif //WG_POINTERS_DOT_H

