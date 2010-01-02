#ifndef	WG_REFCOUNTEDPOOLED_DOT_H
#define WG_REFCOUNTEDPOOLED_DOT_H


//____ WgRefCountedPooled _____________________________________________________

class WgRefCountedPooled
{
friend class WgSmartPtrPooledImpl;

public:
	WgRefCountedPooled(WgMemPool * pPool) {m_ref = 0;m_pPool = pPool; }
	virtual ~WgRefCountedPooled() = 0;

private:
	Uint32		m_ref;
	WgMemPool * m_pPool;
};

#endif //WG_REFCOUNTEDPOOLED_DOT_H
