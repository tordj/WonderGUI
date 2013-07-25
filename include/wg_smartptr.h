#ifndef	WG_SMARTPTR_DOT_H
#define	WG_SMARTPTR_DOT_H

#ifndef WG_MEMPOOL_DOT_H
#	include <wg_mempool.h>
#endif

#ifndef	WG_OBJECT_DOT_H
#	include <wg_object.h>
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
		if( m_pHub && m_pHub->pObj )
			return static_cast<T*>(m_pHub->pObj);
		else
			return reinterpret_cast<T*>(0);
	}
};


#endif //WG_SMARTPTR_DOT_H

