#ifndef	WG_SMARTPTR_DOT_H
#define	WG_SMARTPTR_DOT_H

#ifndef WG_MEMPOOL_DOT_H
#	include <wg_mempool.h>
#endif

#ifndef	WG_REFCOUNTED_DOT_H
#	include <wg_refcounted.h>
#endif


//___ WgSmartPtrImpl __________________________________________________________

class WgSmartPtrImpl
{
public:

	WgSmartPtrImpl(WgRefCounted * p)
	{
		m_pObj = p;
		if( p )
			p->m_ref++;
	}

	~WgSmartPtrImpl()
	{
		if( m_pObj )
		{
			m_pObj->m_ref--;
			if( m_pObj->m_ref == 0 )
				delete m_pObj;
		}
	}

	void copy( WgSmartPtrImpl const & r )
	{
		if( m_pObj != r.m_pObj )
		{
			if( m_pObj )
			{
				m_pObj->m_ref--;
				if( m_pObj->m_ref == 0 )
					delete m_pObj;
			}

			m_pObj = r.m_pObj;
			if( m_pObj )
				m_pObj->m_ref++;
		}
	}

protected:
	WgRefCounted * m_pObj;
};

//____ WgSmartPtr _____________________________________________________________

template<class T> class WgSmartPtr : private WgSmartPtrImpl
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



//____ WgSmartPtrPooled _______________________________________________________

template<class T> class WgSmartPtrPooled
{
public:
	WgSmartPtrPooled(T* p=0)
	{
		m_pObj = p;
		if( p )
			((WgRefCountedPooled*)p)->m_ref++;
	};

	WgSmartPtrPooled(const WgSmartPtrPooled<T>& r)
	{
		m_pObj = r.m_pObj;
		if( m_pObj )
			((WgRefCountedPooled*)m_pObj)->m_ref++;
	}

	~WgSmartPtrPooled()
	{
		if( m_pObj )
		{
			((WgRefCountedPooled*)m_pObj)->m_ref--;
			if( ((WgRefCountedPooled*)m_pObj)->m_ref == 0 )
			{
				m_pObj->~T();
				((WgRefCountedPooled*)m_pObj)->m_pPool->freeEntry(m_pObj);
			}
		}
	};


    WgSmartPtrPooled<T> & operator=( WgSmartPtrPooled<T> const & r)
	{
		if( m_pObj != r.m_pObj )
		{
			if( m_pObj )
			{
				((WgRefCountedPooled*)m_pObj)->m_ref--;

				if( ((WgRefCountedPooled*)m_pObj)->m_ref == 0 )
				{
					m_pObj->~T();
					((WgRefCountedPooled*)m_pObj)->m_pPool->freeEntry(m_pObj);
				}
			}

			m_pObj = r.m_pObj;
			if( m_pObj )
				((WgRefCountedPooled*)m_pObj)->m_ref++;
		}
		return *this;
	}

	inline T & operator*() const { return * m_pObj; }
	inline T * operator->() const{ return m_pObj; }

	inline bool operator==(const WgSmartPtrPooled<T>& other) const { return m_pObj == other.m_pObj; }
	inline bool operator!=(const WgSmartPtrPooled<T>& other) const { return m_pObj != other.m_pObj; }

	inline operator bool() const { return (m_pObj != 0); }

	inline T * GetRealPtr() const { return (T*) m_pObj; }

private:
	T * m_pObj;
};



#endif //WG_SMARTPTR_DOT_H

