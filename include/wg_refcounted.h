#ifndef	WG_REFCOUNTED_DOT_H
#define WG_REFCOUNTED_DOT_H


//____ WgRefCounted ___________________________________________________________

class WgRefCounted
{
friend class WgSmartPtrImpl;
template<class T> friend class WgSmartPtr;

public:
	WgRefCounted() {m_ref = 0;}
	virtual ~WgRefCounted() {};

private:
	Uint32	m_ref;
};


//____ WgRefCountedPooled _____________________________________________________

class WgRefCountedPooled
{
	template<class T> friend class WgSmartPtrPooled;

public:
	WgRefCountedPooled(WgMemPool * pPool) {m_ref = 0;m_pPool = pPool; }
	virtual ~WgRefCountedPooled() {};

private:
	Uint32		m_ref;
	WgMemPool * m_pPool;
};


#endif //WG_REFCOUNTED_DOT_H
