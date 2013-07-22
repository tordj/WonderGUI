#ifndef	WG_SMARTPTR_DOT_H
#define	WG_SMARTPTR_DOT_H

#ifndef WG_MEMPOOL_DOT_H
#	include <wg_mempool.h>
#endif

#ifndef	WG_OBJECT_DOT_H
#	include <wg_object.h>
#endif


//___ WgSmartPtrImpl __________________________________________________________

class WgSmartPtrImpl
{
public:

	WgSmartPtrImpl(WgObject * p)
	{
		m_pObj = p;
		if( p )
			p->m_refCount++;
	}

	~WgSmartPtrImpl()
	{
		if( m_pObj )
		{
			m_pObj->m_refCount--;
			if( m_pObj->m_refCount == 0 )
				delete m_pObj;
		}
	}

	void copy( WgSmartPtrImpl const & r )
	{
		if( m_pObj != r.m_pObj )
		{
			if( m_pObj )
			{
				m_pObj->m_refCount--;
				if( m_pObj->m_refCount == 0 )
					delete m_pObj;
			}

			m_pObj = r.m_pObj;
			if( m_pObj )
				m_pObj->m_refCount++;
		}
	}

protected:
	WgObject * m_pObj;
};


//____ WgSmartPtr _____________________________________________________________

template<class T> class WgSmartPtr : protected WgSmartPtrImpl
{
public:
	WgSmartPtr(T* p=0) : WgSmartPtrImpl( p ) {};
	WgSmartPtr(const WgSmartPtr<T>& r) : WgSmartPtrImpl( r.m_pObj ) {};
	~WgSmartPtr() {};


    inline WgSmartPtr<T> & operator=( WgSmartPtr<T> const & r)
	{
		copy( r );
		return *this;
	}

	inline T & operator*() const { return * (T*) m_pObj; }
	inline T * operator->() const{ return (T*) m_pObj; }

	inline bool operator==(const WgSmartPtr<T>& other) const { return m_pObj == other.m_pObj; }
	inline bool operator!=(const WgSmartPtr<T>& other) const { return m_pObj != other.m_pObj; }

	inline operator bool() const { return (m_pObj != 0); }

	inline T * GetRealPtr() const { return (T*) m_pObj; }
};


//____ WgSmartChildPtr ________________________________________________________

template<class T,class P> class WgSmartChildPtr : public P
{
public:
	WgSmartChildPtr(T* p=0) : P( p ) {};
	WgSmartChildPtr(const WgSmartChildPtr<T,P>& r) : P( (T*) r.m_pObj ) {};
	~WgSmartChildPtr() {};

/*
    inline WgSmartChildPtr<T,P> & operator=( WgSmartChildPtr<T,P> const & r)
	{
		copy( r );
		return *this;
	}
*/
	inline T & operator*() const { return * (T*) this->m_pObj; }
	inline T * operator->() const{ return (T*) this->m_pObj; }

	inline bool operator==(const WgSmartChildPtr<T,P>& other) const { return this->m_pObj == other.m_pObj; }
	inline bool operator!=(const WgSmartChildPtr<T,P>& other) const { return this->m_pObj != other.m_pObj; }

//	inline operator bool() const { return (this->m_pObj != 0); }

	inline T * GetRealPtr() const { return (T*) this->m_pObj; }
};





//____ WgPoolSmartPtr _______________________________________________________

template<class T> class WgPoolSmartPtr
{
public:
	WgPoolSmartPtr(T* p=0)
	{
		m_pObj = p;
		if( p )
			((WgPoolObject*)p)->m_refCount++;
	};

	WgPoolSmartPtr(const WgPoolSmartPtr<T>& r)
	{
		m_pObj = r.m_pObj;
		if( m_pObj )
			((WgPoolObject*)m_pObj)->m_ref++;
	}

	~WgPoolSmartPtr()
	{
		if( m_pObj )
		{
			((WgPoolObject*)m_pObj)->m_ref--;
			if( ((WgPoolObject*)m_pObj)->m_ref == 0 )
			{
				m_pObj->~T();
				((WgPoolObject*)m_pObj)->m_pPool->FreeEntry(m_pObj);
			}
		}
	};


    WgPoolSmartPtr<T> & operator=( WgPoolSmartPtr<T> const & r)
	{
		if( m_pObj != r.m_pObj )
		{
			if( m_pObj )
			{
				((WgPoolObject*)m_pObj)->m_ref--;

				if( ((WgPoolObject*)m_pObj)->m_ref == 0 )
				{
					m_pObj->~T();
					((WgPoolObject*)m_pObj)->m_pPool->FreeEntry(m_pObj);
				}
			}

			m_pObj = r.m_pObj;
			if( m_pObj )
				((WgPoolObject*)m_pObj)->m_ref++;
		}
		return *this;
	}

	inline T & operator*() const { return * m_pObj; }
	inline T * operator->() const{ return m_pObj; }

	inline bool operator==(const WgPoolSmartPtr<T>& other) const { return m_pObj == other.m_pObj; }
	inline bool operator!=(const WgPoolSmartPtr<T>& other) const { return m_pObj != other.m_pObj; }

	inline operator bool() const { return (m_pObj != 0); }

	inline T * GetRealPtr() const { return (T*) m_pObj; }

private:
	T * m_pObj;
};


//____ WgWeakPtrImpl __________________________________________________________

class WgWeakPtrImpl
{
public:
	WgWeakPtrImpl() { m_pHub = 0; }
	WgWeakPtrImpl( WgObject * pObj );
	~WgWeakPtrImpl();

	void copy( WgWeakPtrImpl const & r );

	WgWeakPtrHub * m_pHub;
};


//____ WgWeakPtr ______________________________________________________________

template<class T> class WgWeakPtr : protected WgWeakPtrImpl
{
public:
	WgWeakPtr() {}
	WgWeakPtr( T * pObj ) : WgWeakPtrImpl( pObj ) {}

	WgWeakPtr(const WgWeakPtr<T>& r)
	{

		m_pHub = r.m_pHub;
		if( m_pHub )
			m_pHub->refCnt++;
	}

	~WgWeakPtr() {}


    inline WgWeakPtr<T> & operator=( WgWeakPtr<T> const & r)
	{
		copy( r );
		return *this;
	}

	inline T & operator*() const { return * GetRealPtr(); }
	inline T * operator->() const { return GetRealPtr(); }

	//TODO: Fix so that we get right value if both are null-pointers, but have different hubs.
	inline bool operator==(const WgWeakPtr<T>& other) const { return m_pHub == other.m_pHub; }
	inline bool operator!=(const WgWeakPtr<T>& other) const { return m_pHub != other.m_pHub; }
	inline bool operator<(const WgWeakPtr<T>& other) const { return m_pHub < other.m_pHub ? true : false; }
	inline bool operator>(const WgWeakPtr<T>& other) const { return m_pHub > other.m_pHub ? true : false; }
	inline bool operator<=(const WgWeakPtr<T>& other) const { return m_pHub <= other.m_pHub ? true : false; }
	inline bool operator>=(const WgWeakPtr<T>& other) const { return m_pHub >= other.m_pHub ? true : false; }

	inline operator bool() const { return (m_pHub != 0 && m_pHub->pObj != 0); }

	inline T * GetRealPtr() const
	{
		if( m_pHub && m_pHub->pObj )
			return static_cast<T*>(m_pHub->pObj);
		else
			return reinterpret_cast<T*>(0);
	}
};

//____ WgWeakChildPtr _________________________________________________________

template<class T,class P> class WgWeakChildPtr : public P
{
public:
	WgWeakChildPtr(T* p=0) : P( p ) {};
	WgWeakChildPtr(const WgWeakChildPtr<T,P>& r) : P( r.GetRealPtr() ) {};
	~WgWeakChildPtr() {};

	inline T & operator*() const { return * GetRealPtr(); }
	inline T * operator->() const{ return GetRealPtr(); }

	inline bool operator==(const WgWeakChildPtr<T,P>& other) const { return this->m_pHub == other.m_pHub; }
	inline bool operator!=(const WgWeakChildPtr<T,P>& other) const { return this->m_pHub != other.m_pHub; }

//	inline operator bool() const { return (this->m_pObj != 0); }

	inline T * GetRealPtr() const
	{
		if( m_pHub && m_pHub->pObj )
			return static_cast<T*>(m_pHub->pObj);
		else
			return reinterpret_cast<T*>(0);
	}
};


typedef class WgSmartPtr<WgObject> WgObjectPtr;
typedef class WgWeakPtr<WgObject> WgObjectWeakPtr;


/*

template<typename T> T WgCast(const WgObjectPtr& p)
{
	if(p && p->IsInstanceOf(T::CLASSNAME) )
		return T(p.GetRealPtr());
	else
		return T(0);
}

template<typename DestT,typename SrcT>
inline WgCast(const WgObjectPtr& pObj)
{
	if( !pObj )
		return T(0);



	WgObject * p = pObj.GetRealPtr();



	return T(static_cast<TpObj.GetRealPtr());
}
*/


#endif //WG_SMARTPTR_DOT_H

