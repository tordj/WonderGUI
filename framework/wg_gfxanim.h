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

#ifndef WG_BLOCKSET_DOT_H
#	include <wg_blockset.h>
#endif

#include <wg3_surface.h>

class WgGfxFrame;


#define MAX_ANIM_ALT 5

//____ Class WgGfxAnim ________________________________________________________

class	WgGfxAnim : public WgAnim
{
public:
	WgGfxAnim();
	WgGfxAnim( WgSize size, WgBorders gfxBorders = WgBorders(), Uint32 blockFlags = 0 );

	void		SetSize( WgSize size );
	WgSize		Size(int scale = WG_SCALE_BASE) const;

	void		SetAlternative( int alt, int activationScale, WgSize size, WgBorders gfxBorders );

	void		SetGfxBorders( WgBorders borders );
	WgBorders	GfxBorders() const { return m_borders[0]; }

	void		SetBlockFlags( Uint32 flags );
	Uint32		BlockFlags() const { return m_blockFlags; }

    bool		InsertFrame( int pos, wg::Surface * pSurf, WgCoord ofs, int duration );
    bool		InsertFrame( WgGfxFrame * pBefore, wg::Surface * pSurf, WgCoord ofs, int duration );
    bool		AddFrame( wg::Surface * pSurf, WgCoord ofs, int duration );
    int			AddFrames( wg::Surface * pSurf, WgCoord arrayOfs, WgSize arraySize, int duration, int nFrames = 0, WgSize spacing = WgSize() );
    int			AddFrames( wg::Surface * pSurf, int duration, int nFrames = 0, WgSize spacing = WgSize() );

    bool		SetAltFrame( int pos, int alt, wg::Surface * pSurf, WgCoord ofs );

    int			SetAltFrames( int frameOffset, int scaleFactor, wg::Surface * pSurf, WgCoord arrayOfs, WgSize arraySize, int nFrames = 0, WgSize spacing = WgSize() );

	WgGfxFrame * GetFrame( int64_t ticks, WgGfxFrame * pProximity = 0 ) const;

	WgGfxFrame * GetFirstFrame(void) {return (WgGfxFrame *) WgAnim::_firstKeyFrame(); };
	WgGfxFrame * GetLastFrame(void) {return (WgGfxFrame *) WgAnim::_lastKeyFrame(); };

	WgBlock		 GetBlock( int64_t tick, int scale, WgGfxFrame * pProximity = 0 ) const;

protected:

	int			m_activationScale[MAX_ANIM_ALT];		// Ńeeds to be in increasing order 
	WgSize		m_size[MAX_ANIM_ALT];
	WgBorders	m_borders[MAX_ANIM_ALT];
	Uint32		m_blockFlags;
};

//____ Class WgGfxFrame _______________________________________________________

class WgGfxFrame : public WgKeyFrame
{
public:
	WgGfxFrame();


	// Derived from WgKeyFrame: Uint32	timestamp

	WgGfxFrame * GetNext(void) {return (WgGfxFrame *) WgKeyFrame::Next();};
	WgGfxFrame * GetPrev(void) {return (WgGfxFrame *) WgKeyFrame::Prev();};

    wg::Surface_p pSurf[MAX_ANIM_ALT];
	WgCoord		ofs[MAX_ANIM_ALT];
};

#endif //WG_GFXANIM_DOT_H
