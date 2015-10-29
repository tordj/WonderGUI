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
	
	class GfxDevice;
	
	class Caret2;
	typedef	WgStrongPtr<Caret2,Object_p>		Caret2_p;
	typedef	WgWeakPtr<Caret2,Object_wp>		Caret2_wp;
	
	
	enum class CaretMode
	{
		Insert,
		Overwrite,
		Eol
	};
	
	
	class Caret2 : public Object
	{
	public:
		static Caret2_p	create() { return Caret2_p(new Caret2()); }
	
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static Caret2_p	cast( const Object_p& pObject );
	
		virtual bool		setCycleLength( int millisec );
		inline int			cycleLength() const { return m_cycleLength; }
	
		virtual void		setGlyph( uint16_t glyph, int size, int advance, int ascend, int descend );
		virtual void		setMode( CaretMode mode );
		virtual int			eolWidth( int size, int advance, int ascend, int descend ) const;
		inline CaretMode	mode() const { return m_mode; }
		virtual void		tick( int millisec );
		inline bool			needToRender() const { return m_bNeedToRender; }
		virtual Rect		dirtyRect( Coord pos ) const;
		virtual void		render( GfxDevice * pDevice, Coord pos, const Rect& clip );
	
	protected:
		Caret2();
	
	
		uint16_t		m_glyph;
		int			m_glyphSize;
		int			m_glyphAdvance;
		int			m_glyphAscend;
		int			m_glyphDescend;
	
		CaretMode	m_mode;
		int			m_ticks;
		int			m_cycleLength;
		bool		m_bNeedToRender;
	
	};
	
	
	

} // namespace wg
#endif //WG_CARET_DOT_H
