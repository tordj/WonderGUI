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
#include <wg_blob.h>

const char WgBlob::CLASSNAME[] = {"Blob"};


//____ Create _________________________________________________________________
/**
 * @brief Create an empty blob of the specified size.
 * 
 * @param bytes Bytes to allocate.
 * 
 * Allocates a memory buffer of the given size and wraps it into a Blob.
 * The memory buffer will not move and can be accessed randomly at any
 * time. When the last reference to the Blob is destroyed, the memory
 * buffer is released.
 */

WgBlobPtr WgBlob::Create( int bytes )
{
	WgBlob * pBlob = new(bytes) WgBlob( bytes );
	return WgBlobPtr(pBlob);
}

/**
 * @brief Wrap a non-WonderGUI object or any set of data into a Blob
 * 
 * @param pData			Pointer at the data to be considered the content of the Blob.
 * 
 * @param pDestructor 	Pointer at a function that is called to destroy the data
 * 						when the Blob is destroyed.
 * 
 * This method creates a Blob that refers to the object or data structure given as its content.
 * When the Blob is destroyed, it will make a call to pDestructor with pData as parameter. It will
 * then be up to pDestructor to actually destroy the data (or not).
 * 
 * Since the size of the object/data is unknown to the blob, a call to Size() will return 0.
 * 
 */


WgBlobPtr WgBlob::Create(void * pData, void(*pDestructor)(void*) )
{
	WgBlob * pBlob = new(0) WgBlob( pData, pDestructor );
	return WgBlobPtr(pBlob);
}



//____ Constructor ____________________________________________________________

WgBlob::WgBlob( int size )
{
	m_size = size;
	m_pContent = ((char*)this) + sizeof(this);
	m_pDestructor = 0;
}

WgBlob::WgBlob( void * pData, void(*pDestructor)(void*) )
{
	m_size = 0;
	m_pContent = pData;
	m_pDestructor = pDestructor;
}


//____ Destructor _____________________________________________________________

WgBlob::~WgBlob()
{
	if( m_pDestructor )
		m_pDestructor( m_pContent );
}

//____ IsInstanceOf() _________________________________________________________

bool WgBlob::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgObject::IsInstanceOf(pClassName);
}

//____ ClassName() ____________________________________________________________

const char * WgBlob::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ Cast() _________________________________________________________________

WgBlobPtr WgBlob::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgBlobPtr( static_cast<WgBlob*>(pObject.GetRealPtr()) );

	return 0;
}
