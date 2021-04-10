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
#ifndef WG_PATCHES_DOT_H
#define WG_PATCHES_DOT_H
#pragma once

#include <limits.h>

#include <wg_geo.h>

namespace wg
{

	class Patches
	{
	public:

		//.____ Creation ______________________________________________________

		Patches();
		Patches( int startCapacity );
		Patches( RectSPX * pArray, int capacity );
		Patches(const Patches& source);
		Patches(const Patches& source, const RectSPX& trim);
		~Patches();

		//.____ Control ________________________________________________________

		bool			setCapacity( int capacity );
//		bool			setArray( Rect * pArray, int capacity );
		inline void		clear() { m_size = 0; }

		inline void		add( const RectSPX& rect ) { if( rect.w > 0 && rect.h > 0 ) _add( rect, 0 ); }						// Adds the area
		void			add( const Patches * pSource, int ofs = 0, int len = INT_MAX );

		void			sub( const RectSPX& rect );											// Subtracts the area
		void			sub( const Patches * pSource, int ofs = 0, int len = INT_MAX );

		inline void		push( const RectSPX& rect );											// Adds the rect (no optimizations, overlap may occur).
		int				push( const Patches * pSource, int ofs = 0, int len = INT_MAX );
		inline RectSPX	pop();																// Pops last rect from the patches.

		void			trimPush(const Patches& source, const RectSPX& trim);					// Adds the rects (no optimizations, overlap may occur).

		void			remove( int ofs );													// Deletes specific rect from the patches.
		int				remove( int ofs, int len );											// Deletes range of rects from the patches.

		//.____ Misc __________________________________________________________

		void			clip( const RectSPX& clip );
		RectSPX			getUnion() const;

		int				repair();															// Fixes any overlap that might have resulted from push()
		int				optimize();															// Combines small patches into larger ones where possible

		//.____ Content ________________________________________________________

		const RectSPX *	begin() const { return m_pFirst; }
		const RectSPX *	end() const { return m_pFirst + m_size; }
		int				size() const { return m_size; }
		int				capacity() const { return m_capacity; }
		bool			isEmpty() const { return (m_size == 0); }

	private:
		const static int	c_defaultCapacity = 64;

		void		_add( const RectSPX& rect, int startOffset );
		void		_expandMem( int spaceNeeded );

		RectSPX * 	m_pFirst;
		int			m_size;
		int			m_capacity;
		bool		m_bOwnsArray;
	};



	void Patches::push( const RectSPX& rect )
	{
		if(m_size==m_capacity)
			_expandMem(1);
		m_pFirst[m_size++]=rect;
	}

	RectSPX Patches::pop()
	{
		if( m_size > 0 )
			return m_pFirst[--m_size];
		else
			return RectSPX();
	}



} // namespace wg
#endif //WG_PATCHES_DOT_H
