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

namespace wg 
{
	
	class WgBlob;
	typedef	WgStrongPtr<WgBlob,WgObject_p>		WgBlob_p;
	typedef	WgWeakPtr<WgBlob,WgObject_wp>	WgBlob_wp;
	
	
	/**
	 * @brief Reference counted container of arbitrary data.
	 * 
	 * WgBlob is a container that can be used to wrap a non-WonderGUI
	 * object or any set of data into a reference counted WonderGUI object. 
	 * 
	 * When the Blob is destroyed, the external object is destroyed or the memory area
	 * released.
	 */
	
	class WgBlob : public WgObject
	{
	public:
		static WgBlob_p	create( int bytes );
		static WgBlob_p	create( void * pData, void(*pDestructor)(void*) );
	
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static WgBlob_p	cast( const WgObject_p& pObject );
	    
		inline int		size() const { return m_size; }			///< @brief Get the size of the blobs content.
																///<
																///< Get the size of the blobs content.
																///< The size of the content can only be retrieved if known by the blob.
																///< @return Size of blob content or 0 if unknown.
		void *			content() { return m_pContent; }		///< @brief Get pointer to the content of the blob.
																///<
																///< Get a raw pointer to the content of the blob, which is either the object
																///< wrapped or beginning of the reserved memory area.
																///< @return Pointer to content of the blob.
	
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
	

} // namespace wg
#endif //WG_BLOB_DOT_H
