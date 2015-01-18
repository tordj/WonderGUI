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

class WgGfxDevice;

class WgCaret2;
typedef	WgStrongPtr<WgCaret2,WgObjectPtr>		WgCaret2Ptr;
typedef	WgWeakPtr<WgCaret2,WgObjectWeakPtr>		WgCaret2WeakPtr;


enum WgCaretMode
{
	WG_CARET_INSERT,
	WG_CARET_OVERWRITE,
	WG_CARET_EOL
};


class WgCaret2 : public WgObject
{
public:
	static WgCaret2Ptr	Create() { return WgCaret2Ptr(new WgCaret2()); }

	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgCaret2Ptr	Cast( const WgObjectPtr& pObject );

	virtual bool		SetCycleLength( int millisec );
	inline int			CycleLength() const { return m_cycleLength; }

	virtual void		SetGlyph( Uint16 glyph, int size, int advance, int ascend, int descend );
	virtual void		SetMode( WgCaretMode mode );
	virtual int			EOLWidth( int size, int advance, int ascend, int descend ) const;
	inline WgCaretMode	Mode() const { return m_mode; }
	virtual void		Tick( int millisec );
	inline bool			NeedToRender() const { return m_bNeedToRender; }
	virtual WgRect		DirtyRect( WgCoord pos ) const;
	virtual void		Render( WgGfxDevice * pDevice, WgCoord pos, const WgRect& clip );

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



#endif //WG_CARET_DOT_H
