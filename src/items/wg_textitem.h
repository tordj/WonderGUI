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

#ifndef	WG_TEXTITEM_DOT_H
#define WG_TEXTITEM_DOT_H

#ifndef	WG_TYPES_DOT_H
#	include <wg_types.h>
#endif

#ifndef WG_TEXTBASEITEM_DOT_H
#	include <wg_textbaseitem.h>
#endif


#ifndef WG_BASE_DOT_H
#	include <wg_base.h>
#endif

namespace wg 
{
	
	class String;
	class CharSeq;
	class CharBuffer;
		
	//____ TextItem __________________________________________________________________
	
	class TextItem : public TextBaseItem
	{
	public:
		TextItem( Widget * pWidget );
	
		virtual void		clear();
	
		virtual void		set( const CharSeq& seq );
		virtual void		set( const CharBuffer * buffer );
		virtual void		set( const String& str );
	
		virtual int			append( const CharSeq& seq );
		virtual int			insert( int ofs, const CharSeq& seq );
		virtual int			replace( int ofs, int nDelete, const CharSeq& seq );
		virtual int			remove( int ofs, int len );
	
	
		inline int			length() const { return m_charBuffer.length(); }
		inline bool			isEmpty() const { return length()==0?true:false; }
		TextLink_p			markedLink() const;
	
		
		virtual void		setCharStyle( const TextStyle_p& pStyle );
		virtual void		setCharStyle( const TextStyle_p& pStyle, int ofs, int len);

		virtual void		clearCharStyle();
		virtual void		clearCharStyle( int ofs, int len );

		virtual void		receive( const Msg_p& pMsg );
	
	protected:
		TextLink_p			m_pMarkedLink;	// Character offset for beginning of marked or focused link
	};
	

} // namespace wg
#endif //WG_TEXTITEM_DOT_H
