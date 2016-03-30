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

#ifndef	WG_CARET_DOT_H
#define WG_CARET_DOT_H

#ifndef WG_GEO_DOT_H
#	include <wg_geo.h>
#endif

#ifndef WG_POINTERS_DOT_H
#	include <wg_pointers.h>
#endif

namespace wg 
{
	
	class GfxDevice;
	
	class Caret;
	typedef	StrongPtr<Caret,Object_p>		Caret_p;
	typedef	WeakPtr<Caret,Object_wp>		Caret_wp;
	
	
	enum class CaretMode
	{
		Insert,
		Overwrite,
		Eol
	};
	
	
	class Caret : public Object
	{
	public:
		static Caret_p	create() { return Caret_p(new Caret()); }
	
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static Caret_p	cast( const Object_p& pObject );
	
		virtual bool		setCycleLength( int millisec );
		inline int			cycleLength() const { return m_cycleLength; }
	
		virtual void		setCellMetrics( const Rect& cell );			// Rectangle of character cell with the carret relative to characters postion.
		virtual void		setMode( CaretMode mode );
		virtual int			eolWidth( const Rect& eolCellMetrics ) const;
		inline CaretMode	mode() const { return m_mode; }
		virtual void		tick( int millisec );
		inline bool			needToRender() const { return m_bNeedToRender; }
		virtual Rect		dirtyRect( Coord pos ) const;
		virtual void		render( GfxDevice * pDevice, Coord pos, const Rect& clip );
	
	protected:
		Caret();
		
		Rect		m_cellMetrics;
	
		CaretMode	m_mode;
		int			m_ticks;
		int			m_cycleLength;
		bool		m_bNeedToRender;
	
	};
	

} // namespace wg
#endif //WG_CARET_DOT_H
