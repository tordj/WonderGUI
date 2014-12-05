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

class WgObjectWeakPtr;

//____ WgObjectPtr _____________________________________________________________

class WgObjectPtr
{
public:
	WgObjectPtr(WgObject* p=0)
	{
		m_pObj = p;
		if( p )
			p->m_refCount++;
	}

	WgObjectPtr(const WgObjectPtr& r)
	{
		m_pObj = r.m_pObj;
		if( m_pObj )
			m_pObj->m_refCount++;
	}

	WgObjectPtr( const WgObjectWeakPtr& r );

	~WgObjectPtr()
	{
		if( m_pObj )
		{
			m_pObj->m_refCount--;
			if( m_pObj->m_refCount == 0 )
				delete m_pObj;
		}
	}


    inline WgObjectPtr & operator=( WgObjectPtr const & r)
	{
		copy( r );
		return *this;
	}

	inline WgObject& operator*() const { return * m_pObj; }
	inline WgObject* operator->() const{ return m_pObj; }

	inline bool operator==(const WgObjectPtr& other) const { return m_pObj == other.m_pObj; }
	inline bool operator!=(const WgObjectPtr& other) const { return m_pObj != other.m_pObj; }

	inline operator bool() const { return (m_pObj != 0); }

	inline WgObject * RawPtr() const { return m_pObj; }

protected:
	void copy( WgObjectPtr const & r )
	{
		if( m_pObj != r.m_pObj )
		{
			if( m_pObj )
			{
				m_pObj->m_refCount--;
				if( m_pObj->m_refCount == 0 )
					m_pObj->_destroy();
			}

			m_pObj = r.m_pObj;
			if( m_pObj )
				m_pObj->m_refCount++;
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
//	WgStrongPtr(const WgWeakPtr<T,P>& r) : P( (T*) r.RawPtr() ) {};
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

	inline T * RawPtr() const { return (T*) this->m_pObj; }
};


//____ WgObjectWeakPtr ______________________________________________________________

class WgObjectWeakPtr
{
public:
	WgObjectWeakPtr() : m_pHub(0) {}
	WgObjectWeakPtr( WgObject * pObj );
	WgObjectWeakPtr(const WgObjectPtr& r);
	WgObjectWeakPtr(const WgObjectWeakPtr& r)
	{

		m_pHub = r.m_pHub;
		if( m_pHub )
			m_pHub->refCnt++;
	}

	~WgObjectWeakPtr();

    inline WgObjectWeakPtr& operator=( WgObjectWeakPtr const & r)
	{
		copy( r );
		return *this;
	}

	inline WgObject& operator*() const { return * RawPtr(); }
	inline WgObject * operator->() const { return RawPtr(); }

	//TODO: Fix so that we get right value if both are null-pointers, but have different hubs.
	inline bool operator==(const WgObjectWeakPtr& other) const { return m_pHub == other.m_pHub; }
	inline bool operator!=(const WgObjectWeakPtr& other) const { return m_pHub != other.m_pHub; }
	inline bool operator<(const WgObjectWeakPtr& other) const { return m_pHub < other.m_pHub ? true : false; }
	inline bool operator>(const WgObjectWeakPtr& other) const { return m_pHub > other.m_pHub ? true : false; }
	inline bool operator<=(const WgObjectWeakPtr& other) const { return m_pHub <= other.m_pHub ? true : false; }
	inline bool operator>=(const WgObjectWeakPtr& other) const { return m_pHub >= other.m_pHub ? true : false; }

	inline operator bool() const { return (m_pHub != 0 && m_pHub->pObj != 0); }

	inline WgObject * RawPtr() const
	{
		if( m_pHub )
			return m_pHub->pObj;
		else
			return 0;
	}

	void copy( WgObjectWeakPtr const & r );

	WgWeakPtrHub * m_pHub;

};

//____ WgWeakPtr _________________________________________________________

template<class T,class P> class WgWeakPtr : public P
{
public:
	WgWeakPtr(T* p=0) : P( p ) {};
	WgWeakPtr(const WgWeakPtr<T,P>& r) : P( r.RawPtr() ) {};
//	WgWeakPtr(const WgStrongPtr<T,P>& r) : P( r.RawPtr() ) {};
	~WgWeakPtr() {};

	inline T & operator*() const { return * RawPtr(); }
	inline T * operator->() const{ return RawPtr(); }

	inline bool operator==(const WgWeakPtr<T,P>& other) const { return this->m_pHub == other.m_pHub; }
	inline bool operator!=(const WgWeakPtr<T,P>& other) const { return this->m_pHub != other.m_pHub; }

//	inline operator bool() const { return (this->m_pObj != 0); }

	inline T * RawPtr() const
	{
		if( this->m_pHub && this->m_pHub->pObj )
			return static_cast<T*>(this->m_pHub->pObj);
		else
			return reinterpret_cast<T*>(0);
	}
};


//____ WgInterfacePtr _____________________________________________________________

// m_pObj and m_pInterface must both be valid or null.

class WgInterfacePtr
{
public:

	WgInterfacePtr()
	{
		m_pObj = 0;
		m_pInterface = 0;
	}

	WgInterfacePtr(WgObject* pObj, WgInterface* pInterface )
	{
		m_pInterface = pInterface;
		m_pObj = pObj;
		if( pObj )
			pObj->m_refCount++;
	}

	WgInterfacePtr(const WgInterfacePtr& r)
	{
		m_pInterface = r.m_pInterface;
		m_pObj = r.m_pObj;
		if( m_pObj )
			m_pObj->m_refCount++;
	}

	~WgInterfacePtr()
	{
		if( m_pObj )
		{
			m_pObj->m_refCount--;
			if( m_pObj->m_refCount == 0 )
				delete m_pObj;
		}
	}


    inline WgInterfacePtr & operator=( WgInterfacePtr const & r)
	{
		copy( r );
		return *this;
	}

	inline WgInterface& operator*() const { return * m_pInterface; }
	inline WgInterface* operator->() const{ return m_pInterface; }

	inline bool operator==(const WgInterfacePtr& other) const { return m_pInterface == other.m_pInterface; }
	inline bool operator!=(const WgInterfacePtr& other) const { return m_pInterface != other.m_pInterface; }

	inline operator bool() const { return (m_pInterface != 0); }

	inline WgInterface * RawPtr() const { return m_pInterface; }
	inline WgObject * GetRealObjectPtr() const { return m_pObj; }

protected:
	void copy( WgInterfacePtr const & r )
	{
		m_pInterface = r.m_pInterface;
		if( m_pObj != r.m_pObj )
		{
			if( m_pObj )
			{
				m_pObj->m_refCount--;
				if( m_pObj->m_refCount == 0 )
					m_pObj->_destroy();
			}

			m_pObj = r.m_pObj;
			if( m_pObj )
				m_pObj->m_refCount++;
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

	inline T * RawPtr() const { return (T*) this->m_pInterface; }
};


//____ WgInterfaceWeakPtr ______________________________________________________________

class WgInterfaceWeakPtr
{
public:
	WgInterfaceWeakPtr() { m_pHub = 0; m_pInterface = 0; }
	WgInterfaceWeakPtr( WgObject * pObj, WgInterface * pInterface );

	WgInterfaceWeakPtr(const WgInterfaceWeakPtr& r)
	{
		m_pInterface = r.m_pInterface;
		m_pHub = r.m_pHub;
		if( m_pHub )
			m_pHub->refCnt++;
	}

	~WgInterfaceWeakPtr();


    inline WgInterfaceWeakPtr& operator=( WgInterfaceWeakPtr const & r)
	{
		copy( r );
		return *this;
	}

	inline WgInterface& operator*() const { return * RawPtr(); }
	inline WgInterface * operator->() const { return RawPtr(); }

	inline bool operator==(const WgInterfaceWeakPtr& other) const { return m_pInterface == other.m_pInterface; }
	inline bool operator!=(const WgInterfaceWeakPtr& other) const { return m_pInterface != other.m_pInterface; }
	inline bool operator<(const WgInterfaceWeakPtr& other) const { return m_pInterface < other.m_pInterface ? true : false; }
	inline bool operator>(const WgInterfaceWeakPtr& other) const { return m_pInterface > other.m_pInterface ? true : false; }
	inline bool operator<=(const WgInterfaceWeakPtr& other) const { return m_pInterface <= other.m_pInterface ? true : false; }
	inline bool operator>=(const WgInterfaceWeakPtr& other) const { return m_pInterface >= other.m_pInterface ? true : false; }

	inline operator bool() const { return (m_pHub != 0 && m_pHub->pObj != 0); }

	inline WgInterface * RawPtr() const
	{
		if( m_pHub && m_pHub->pObj )
			return m_pInterface;
		else
			return 0;
	}

	void copy( WgInterfaceWeakPtr const & r );

	WgWeakPtrHub *	m_pHub;
	WgInterface *	m_pInterface;

};

//____ WgIWeakPtr _________________________________________________________

template<class T,class P> class WgIWeakPtr : public P
{
public:
	WgIWeakPtr(WgObject* pObj, T* pInterface=0) : P( pObj, pInterface ) {};
	WgIWeakPtr(const WgIWeakPtr<T,P>& r) : P( r.RawPtr(), r.m_pInterface ) {};
	~WgIWeakPtr() {};

	inline T & operator*() const { return * RawPtr(); }
	inline T * operator->() const{ return RawPtr(); }

	inline bool operator==(const WgIWeakPtr<T,P>& other) const { return this->m_pInterface == other.m_pInterface; }
	inline bool operator!=(const WgIWeakPtr<T,P>& other) const { return this->m_pInterface != other.m_pInterface; }

	inline T * RawPtr() const
	{
		if( this->m_pHub && this->m_pHub->pObj )
			return static_cast<T*>(this->m_pInterface);
		else
			return reinterpret_cast<T*>(0);
	}
};



#endif //WG_POINTERS_DOT_H

