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

#ifndef	WG_CURSOR_DOT_H
#define WG_CURSOR_DOT_H

#ifndef	WG_TYPES_DOT_H
#	include <wg_types.h>
#endif

#ifndef WG_GEO_DOT_H
#	include <wg_geo.h>
#endif

class WgGfxAnim;
class WgGlyphSet;


//____ WgCursor _______________________________________________________________

class WgCursor
{
	friend class WgGlyphSet;

public:
	WgCursor();
	WgCursor( WgCursor& in );
	
	enum Mode
	{
		EOL = 0,
		INS,
		OVR
	};
	
	enum ScaleMode
	{
		FIXED_SIZE,					// Size ratio is ignored.
		STRETCH_VERTICALLY,
		STRETCH_SCALED,
		TILE_VERTICALLY,
		TILE_SCALED
	};


	void				setStretchBorders( WgBorders borders );
	void				setScaleMode( ScaleMode mode );
	void				setSizeRatio( float ratio );

	bool				setMode( Mode m, WgGfxAnim * pAnim, WgCord bearing = WgCord(), int advance = 0 );

	void				setBearing( Mode m, WgCord bearing );
	void				setAdvance( Mode m, int advance );
	void				setAnim( Mode m, WgGfxAnim * pAnim );
	
	int					bearingX( Mode m ) const { return m_bearing[m].x; };
	int					bearingY( Mode m ) const { return m_bearing[m].y; };
	WgCord				bearing( Mode m ) const { return m_bearing[m]; }
	int					advance( Mode m ) const { return m_advance[m]; };
	WgGfxAnim * 		anim( Mode m ) const { return m_pAnim[m]; };
	ScaleMode			scaleMode() const { return m_scaleMode; }
	const WgBorders *	stretchBorders() const { return &m_stretchBorders; }
	float				sizeRatio() const { return m_sizeRatio; }

private:
	enum { N_MODES = 3 };

	WgGfxAnim *			m_pAnim[N_MODES];
	WgCord				m_bearing[N_MODES];
	int					m_advance[N_MODES];

	ScaleMode			m_scaleMode;
	WgBorders			m_stretchBorders;
	float				m_sizeRatio;			// ratio <= 1.f. Cursors height relative fonts lineheight.
};





#endif // WG_CURSOR_DOT_H
