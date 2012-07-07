#ifndef	WG_REFCOUNTED_DOT_H
#define WG_REFCOUNTED_DOT_H


//____ WgRefCounted ___________________________________________________________

class WgRefCounted
{
friend class WgSmartPtrImpl;
template<class T> friend class WgSmartPtr;
template<class T> friend class WgRefCountPtr;

protected:
	WgRefCounted() {m_ref = 0;}
	virtual ~WgRefCounted() {};

private:
	Uint32	m_ref;
};


//____ WgRefCountedPooled _____________________________________________________

class WgRefCountedPooled
{
	template<class T> friend class WgSmartPtrPooled;

protected:
	WgRefCountedPooled(WgMemPool * pPool) {m_ref = 0;m_pPool = pPool; }
	virtual ~WgRefCountedPooled() {};

private:
	Uint32		m_ref;
	WgMemPool * m_pPool;
};


//____ WgWeakPtrTarget _________________________________________________________

class WgWeakPtrTarget;
class WgWeakPtrImpl;

class WgWeakPtrHub
{
public:
	int					refCnt;
	WgWeakPtrTarget *	pObj;
};

class WgWeakPtrTarget
{
	friend class WgWeakPtrImpl;
protected:
	WgWeakPtrTarget() : m_pHub(0) {}
	~WgWeakPtrTarget() { if( m_pHub ) m_pHub->pObj = 0; }

private:
	WgWeakPtrHub *	m_pHub;

};

#endif //WG_REFCOUNTED_DOT_H
