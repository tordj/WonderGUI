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

#ifndef	WG_BLOB_DOT_H
#define	WG_BLOB_DOT_H

#include <stddef.h>

#ifndef WG_POINTERS_DOT_H
#	include <wg_pointers.h>
#endif

class WgBlob;
typedef	WgStrongPtr<WgBlob,WgObjectPtr>		WgBlobPtr;
typedef	WgWeakPtr<WgBlob,WgObjectWeakPtr>	WgBlobWeakPtr;

class WgBlob : public WgObject
{
public:
	static WgBlobPtr	Create( int bytes );
	static WgBlobPtr	Create( void * pData, void(*pDestructor)(void*) );

	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgBlobPtr	Cast( const WgObjectPtr& pObject );
    
	inline int		Size() const { return m_size; }
	void *			Content() { return m_pContent; }

protected:
	WgBlob( int bytes );
	WgBlob( void * pData, void(*pDestructor)(void*) );
	virtual ~WgBlob();

	void* operator new (size_t s, int bytes) { return new char[sizeof(WgBlob) + bytes]; }
	void operator delete(void * p, int bytes)  { delete[] (char*) p; }
	void operator delete(void * p)             { delete[] (char*) p; }

	int		m_size;
	void *	m_pContent;
	void(*m_pDestructor)(void*);
};

#endif //WG_BLOB_DOT_H
