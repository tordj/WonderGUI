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

#ifndef WG_ITEXTDISPLAY_DOT_H
#define WG_ITEXTDISPLAY_DOT_H
#pragma once

#include <wg_irotextdisplay.h>
#include <wg_pointers.h>
#include <wg_textlink.h>
#include <wg_font.h>

namespace wg
{

	class Color;
	class CharSeq;
	class String;
	class CharBuffer;

	class ITextDisplay;
	typedef	StrongComponentPtr<ITextDisplay>	ITextDisplay_p;
	typedef	WeakComponentPtr<ITextDisplay>		ITextDisplay_wp;

	/**
	 * @brief Interface for a text component
	 *
	 * Interface that provides read-write access to the text of a text component and methods
	 * to modify its appearance.
	 *
	*/

	class ITextDisplay : public IROTextDisplay
	{
	public:
		/** @private */

		ITextDisplay( CTextDisplay * pComponent ) : IROTextDisplay(pComponent) {};

		//.____ Content _____________________________________________

		inline void				clear() { m_pComponent->clear(); }

		inline void				set( const CharSeq& seq ) { m_pComponent->set(seq); }
		inline void				set( const CharBuffer * pBuffer ) { m_pComponent->set(pBuffer); }
		inline void				set( const String& str ) { m_pComponent->set(str); }

		inline int				append( const CharSeq& seq ) { return m_pComponent->append(seq); }
		inline int				insert( int ofs, const CharSeq& seq ) { return m_pComponent->insert(ofs,seq); }
		inline int				replace( int ofs, int len, const CharSeq& seq ) { return m_pComponent->replace(ofs,len,seq); }
		inline int				erase( int ofs, int len ) { return m_pComponent->erase(ofs,len); }

		//.____ Appearance _____________________________________________

		inline void				setCharStyle( TextStyle * pStyle ) { m_pComponent->setCharStyle(pStyle); }
		inline void				setCharStyle( TextStyle * pStyle, int ofs, int len) { m_pComponent->setCharStyle(pStyle, ofs, len); }

		inline void				clearCharStyle() { m_pComponent->clearCharStyle(); }
		inline void				clearCharStyle( int ofs, int len ) { m_pComponent->clearCharStyle(ofs,len); }

		//.____ Misc __________________________________________________

		inline ITextDisplay_p		ptr() { return ITextDisplay_p(this); }

	};



} // namespace wg
#endif //WG_ITEXTDISPLAY_DOT_H
