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

#ifndef WG_GFXANIM_DOT_H
#define WG_GFXANIM_DOT_H

#ifndef WG_ANIM_DOT_H
#	include <wg_anim.h>
#endif

#ifndef	WG_TYPES_DOT_H
#	include <wg_types.h>
#endif

#ifndef WG_GEO_DOT_H
#	include <wg_geo.h>
#endif

class WgSurface;
class WgGfxFrame;


//____ Class WgGfxAnim ________________________________________________________

class	WgGfxAnim : public WgAnim
{
public:
	WgGfxAnim();
	WgGfxAnim( Uint32 width, Uint32 height );

	void	setWidth( Uint32 width ) { m_width = width; }
	Uint32	width( void ) { return m_width; };
	void	setHeight( Uint32 height ) { m_height = height; }
	Uint32	height( void ) { return m_height; };

	bool	insertFrame( Uint32 pos, WgSurface * pSurf, Uint16 xOfs, Uint16 yOfs, Uint32 duration );
	bool	insertFrame( WgGfxFrame * pBefore, WgSurface * pSurf, Uint16 xOfs, Uint16 yOfs, Uint32 duration );
	bool	addFrame( WgSurface * pSurf, Uint16 xOfs, Uint16 yOfs, Uint32 duration );


	Uint32	addHorrTiledFrames( Uint32 nFrames, WgSurface * pSurf, Uint16 xOfs, Uint16 yOfs, Uint32 duration, int spacing = 0 );
	Uint32	addVertTiledFrames( Uint32 nFrames, WgSurface * pSurf, Uint16 xOfs, Uint16 yOfs, Uint32 duration, int spacing = 0 );


	WgGfxFrame * getFrame( Uint32 tick, WgGfxFrame * pProximity = 0 ) const;

	WgGfxFrame * getFirstFrame(void) {return (WgGfxFrame *) WgAnim::FirstKeyFrame(); };
	WgGfxFrame * getLastFrame(void) {return (WgGfxFrame *) WgAnim::LastKeyFrame(); };


protected:
	Uint32		m_width;
	Uint32		m_height;
};

//____ Class WgGfxFrame _______________________________________________________

class WgGfxFrame : public WgKeyFrame
{
public:
	// Derived from WgKeyFrame: Uint32	timestamp

	WgGfxFrame * getNext(void) {return (WgGfxFrame *) WgKeyFrame::Next();};
	WgGfxFrame * getPrev(void) {return (WgGfxFrame *) WgKeyFrame::Prev();};

	WgSurface	* pSurf;
	WgUCord16		ofs;
};

#endif //WG_GFXANIM_DOT_H
