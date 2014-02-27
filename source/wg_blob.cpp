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

WgBlobPtr WgBlob::Create( int bytes )
{
	WgBlob * pBlob = new(bytes) WgBlob( bytes );
	return WgBlobPtr(pBlob);
}




//____ Constructor ____________________________________________________________

WgBlob::WgBlob( int size )
{
	m_size = size;
	m_pContent = ((char*)this) + sizeof(this);
}

//____ Destructor _____________________________________________________________

WgBlob::~WgBlob()
{
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
