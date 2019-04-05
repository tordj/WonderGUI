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
#pragma once

#include <stddef.h>
#include <functional>

#include <wg_pointers.h>

namespace wg
{

	class Blob;
	typedef	StrongPtr<Blob>		Blob_p;
	typedef	WeakPtr<Blob>		Blob_wp;


	/**
	 * @brief Reference counted container of arbitrary data.
	 *
	 * Blob is a container that can be used to wrap a non-WonderGUI
	 * object or any set of data into a reference counted WonderGUI object.
	 *
	 * When the Blob is destroyed, the external object is destroyed or the memory area
	 * released.
	 */

	class Blob : public Object
	{
	public:
		//.____ Creation __________________________________________

		static Blob_p	create( int size );
		static Blob_p	create( void * pData, std::function<void()> destructor );
		static Blob_p	create( void * pData, int size, std::function<void()> destructor );

		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static Blob_p	cast( Object * pObject );

		//.____ Misc ___________________________________________________________

		inline int		size() const { return m_size; }			///< @brief Get the size of the blobs data.
																///<
																///< Get the size of the blobs data.
																///< The size of the content can only be retrieved if known by the blob.
																///< @return Size of blob data or 0 if unknown.
		void *			data() { return m_pData; }				///< @brief Get pointer to the data of the blob.
																///<
																///< Get a raw pointer to the data of the blob, which is either the object
																///< wrapped or beginning of the reserved memory area.
																///< @return Pointer to data of the blob.

	protected:
		Blob( int bytes );
		Blob( void * pData, int size, std::function<void()> destructor );
		virtual ~Blob();

		void* operator new (size_t s, int bytes) { return new char[sizeof(Blob) + bytes]; }
		void operator delete(void * p, int bytes)  { delete[] (char*) p; }
		void operator delete(void * p)             { delete[] (char*) p; }

		int		m_size;
		void *	m_pData;

		std::function<void()> m_destructor;
	};


} // namespace wg
#endif //WG_BLOB_DOT_H
