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
	
	bool				setMode( Mode m, WgGfxAnim * pAnim, Sint8 ofsX = 0, Sint8 ofsY = 0, Uint8 spacing = 0 );

	void				setOfs( Mode m, Sint8 ofsX, Sint8 ofsY );
	void				setSpacing( Mode m, Uint8 width );
	void				setAnim( Mode m, WgGfxAnim * pAnim );
	
	Sint8				ofsX( Mode m ) const { return m_ofsX[m]; };
	Sint8				ofsY( Mode m ) const { return m_ofsY[m]; };
	Uint8				spacing( Mode m ) const { return m_spacing[m]; };
	WgGfxAnim * 		anim( Mode m ) const { return m_pAnim[m]; };
	
private:
	enum { N_MODES = 3 };

	WgGfxAnim *			m_pAnim[N_MODES];
	Sint8				m_ofsX[N_MODES];
	Sint8				m_ofsY[N_MODES];
	Uint8				m_spacing[N_MODES];
};





#endif // WG_CURSOR_DOT_H
