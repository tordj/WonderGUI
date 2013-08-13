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
#ifndef WG_OBJECT_DOT_H
#define WG_OBJECT_DOT_H

class WgObject;

class WgWeakPtrHub
{
public:
	int					refCnt;
	WgObject *			pObj;
};


class WgObject
{
	friend class WgObjectPtr;
	friend class WgObjectWeakPtr;
	template<class T, class P> friend class WgSmartPtr;
	template<class T, class P> friend class WgWeakPtr;

	friend class WgInterfacePtr;
	friend class WgInterfaceWeakPtr;

public:
	virtual bool		IsInstanceOf( const char * pClassName ) const;
	virtual const char *ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgObjectPtr	Cast( const WgObjectPtr& pObject );				// Provided just for completeness sake.

protected:
	WgObject() : m_refCount(0), m_pWeakPtrHub(0) {}
	virtual ~WgObject() { if( m_pWeakPtrHub ) m_pWeakPtrHub->pObj = 0; }

	virtual void _destroy();			// Pointers should call destroy instead of destructor.

	int				m_refCount;
	WgWeakPtrHub *	m_pWeakPtrHub;
};


#endif //WG_OBJECT_DOT_H
