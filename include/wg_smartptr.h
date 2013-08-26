#ifndef	WG_SMARTPTR_DOT_H
#define	WG_SMARTPTR_DOT_H

#ifndef WG_MEMPOOL_DOT_H
#	include <wg_mempool.h>
#endif

#ifndef	WG_OBJECT_DOT_H
#	include <wg_object.h>
#endif

#ifndef WG_INTERFACE_DOT_H
#	include	<wg_interface.h>
#endif


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

	inline WgObject * GetRealPtr() const { return m_pObj; }

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


//____ WgSmartPtr ________________________________________________________

template<class T,class P> class WgSmartPtr : public P
{
public:
	WgSmartPtr(T* p=0) : P( p ) {};
	WgSmartPtr(const WgSmartPtr<T,P>& r) : P( (T*) r.m_pObj ) {};
	~WgSmartPtr() {};

/*
    inline WgSmartPtr<T,P> & operator=( WgSmartPtr<T,P> const & r)
	{
		copy( r );
		return *this;
	}
*/
	inline T & operator*() const { return * (T*) this->m_pObj; }
	inline T * operator->() const{ return (T*) this->m_pObj; }

	inline bool operator==(const WgSmartPtr<T,P>& other) const { return this->m_pObj == other.m_pObj; }
	inline bool operator!=(const WgSmartPtr<T,P>& other) const { return this->m_pObj != other.m_pObj; }

//	inline operator bool() const { return (this->m_pObj != 0); }

	inline T * GetRealPtr() const { return (T*) this->m_pObj; }
};


//____ WgObjectWeakPtr ______________________________________________________________

class WgObjectWeakPtr
{
public:
	WgObjectWeakPtr() { m_pHub = 0; }
	WgObjectWeakPtr( WgObject * pObj );

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

	inline WgObject& operator*() const { return * GetRealPtr(); }
	inline WgObject * operator->() const { return GetRealPtr(); }

	//TODO: Fix so that we get right value if both are null-pointers, but have different hubs.
	inline bool operator==(const WgObjectWeakPtr& other) const { return m_pHub == other.m_pHub; }
	inline bool operator!=(const WgObjectWeakPtr& other) const { return m_pHub != other.m_pHub; }
	inline bool operator<(const WgObjectWeakPtr& other) const { return m_pHub < other.m_pHub ? true : false; }
	inline bool operator>(const WgObjectWeakPtr& other) const { return m_pHub > other.m_pHub ? true : false; }
	inline bool operator<=(const WgObjectWeakPtr& other) const { return m_pHub <= other.m_pHub ? true : false; }
	inline bool operator>=(const WgObjectWeakPtr& other) const { return m_pHub >= other.m_pHub ? true : false; }

	inline operator bool() const { return (m_pHub != 0 && m_pHub->pObj != 0); }

	inline WgObject * GetRealPtr() const
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
	WgWeakPtr(const WgWeakPtr<T,P>& r) : P( r.GetRealPtr() ) {};
	~WgWeakPtr() {};

	inline T & operator*() const { return * GetRealPtr(); }
	inline T * operator->() const{ return GetRealPtr(); }

	inline bool operator==(const WgWeakPtr<T,P>& other) const { return this->m_pHub == other.m_pHub; }
	inline bool operator!=(const WgWeakPtr<T,P>& other) const { return this->m_pHub != other.m_pHub; }

//	inline operator bool() const { return (this->m_pObj != 0); }

	inline T * GetRealPtr() const
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

	inline WgInterface * GetRealPtr() const { return m_pInterface; }

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

//____ WgISmartPtr ________________________________________________________

template<class T,class P> class WgISmartPtr : public P
{
public:
	WgISmartPtr(WgObject* pObj, T* pInterface=0) : P( pObj, pInterface ) {};
	WgISmartPtr(const WgISmartPtr<T,P>& r) : P( r.pObj, (T*) r.m_pObj ) {};
	~WgISmartPtr() {};

	inline T & operator*() const { return * (T*) this->m_pInterface; }
	inline T * operator->() const{ return (T*) this->m_pInterface; }

	inline bool operator==(const WgISmartPtr<T,P>& other) const { return this->m_pInterface == other.m_pInterface; }
	inline bool operator!=(const WgISmartPtr<T,P>& other) const { return this->m_pInterface != other.m_pInterface; }

	inline T * GetRealPtr() const { return (T*) this->m_pInterface; }
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

	inline WgInterface& operator*() const { return * GetRealPtr(); }
	inline WgInterface * operator->() const { return GetRealPtr(); }

	inline bool operator==(const WgInterfaceWeakPtr& other) const { return m_pInterface == other.m_pInterface; }
	inline bool operator!=(const WgInterfaceWeakPtr& other) const { return m_pInterface != other.m_pInterface; }
	inline bool operator<(const WgInterfaceWeakPtr& other) const { return m_pInterface < other.m_pInterface ? true : false; }
	inline bool operator>(const WgInterfaceWeakPtr& other) const { return m_pInterface > other.m_pInterface ? true : false; }
	inline bool operator<=(const WgInterfaceWeakPtr& other) const { return m_pInterface <= other.m_pInterface ? true : false; }
	inline bool operator>=(const WgInterfaceWeakPtr& other) const { return m_pInterface >= other.m_pInterface ? true : false; }

	inline operator bool() const { return (m_pHub != 0 && m_pHub->pObj != 0); }

	inline WgInterface * GetRealPtr() const
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
	WgIWeakPtr(const WgIWeakPtr<T,P>& r) : P( r.GetRealPtr(), r.m_pInterface ) {};
	~WgIWeakPtr() {};

	inline T & operator*() const { return * GetRealPtr(); }
	inline T * operator->() const{ return GetRealPtr(); }

	inline bool operator==(const WgIWeakPtr<T,P>& other) const { return this->m_pInterface == other.m_pInterface; }
	inline bool operator!=(const WgIWeakPtr<T,P>& other) const { return this->m_pInterface != other.m_pInterface; }

	inline T * GetRealPtr() const
	{
		if( this->m_pHub && this->m_pHub->pObj )
			return static_cast<T*>(this->m_pInterface);
		else
			return reinterpret_cast<T*>(0);
	}
};



#endif //WG_SMARTPTR_DOT_H

