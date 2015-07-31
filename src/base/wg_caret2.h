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

#ifndef	WG_CARET2_DOT_H
#define WG_CARET2_DOT_H

#ifndef WG_GEO_DOT_H
#	include <wg_geo.h>
#endif

#ifndef WG_POINTERS_DOT_H
#	include <wg_pointers.h>
#endif

namespace wg 
{
	
	class WgGfxDevice;
	
	class WgCaret2;
	typedef	WgStrongPtr<WgCaret2,WgObject_p>		WgCaret2_p;
	typedef	WgWeakPtr<WgCaret2,WgObject_wp>		WgCaret2_wp;
	
	
	enum WgCaretMode
	{
		WG_CARET_INSERT,
		WG_CARET_OVERWRITE,
		WG_CARET_EOL
	};
	
	
	class WgCaret2 : public WgObject
	{
	public:
		static WgCaret2_p	create() { return WgCaret2_p(new WgCaret2()); }
	
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static WgCaret2_p	cast( const WgObject_p& pObject );
	
		virtual bool		setCycleLength( int millisec );
		inline int			cycleLength() const { return m_cycleLength; }
	
		virtual void		setGlyph( Uint16 glyph, int size, int advance, int ascend, int descend );
		virtual void		setMode( WgCaretMode mode );
		virtual int			eolWidth( int size, int advance, int ascend, int descend ) const;
		inline WgCaretMode	mode() const { return m_mode; }
		virtual void		tick( int millisec );
		inline bool			needToRender() const { return m_bNeedToRender; }
		virtual WgRect		dirtyRect( WgCoord pos ) const;
		virtual void		render( WgGfxDevice * pDevice, WgCoord pos, const WgRect& clip );
	
	protected:
		WgCaret2();
	
	
		Uint16		m_glyph;
		int			m_glyphSize;
		int			m_glyphAdvance;
		int			m_glyphAscend;
		int			m_glyphDescend;
	
		WgCaretMode	m_mode;
		int			m_ticks;
		int			m_cycleLength;
		bool		m_bNeedToRender;
	
	};
	
	
	

} // namespace wg
#endif //WG_CARET_DOT_H
