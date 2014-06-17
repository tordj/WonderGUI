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

#ifndef WG_SURFACE_DOT_H
#	include	<wg_surface.h>
#endif

class WgGfxFrame;

class WgGfxAnim;
typedef	WgStrongPtr<WgGfxAnim,WgAnimPtr>		WgGfxAnimPtr;
typedef	WgWeakPtr<WgGfxAnim,WgAnimWeakPtr>	WgGfxAnimWeakPtr;

//____ Class WgGfxAnim ________________________________________________________

class	WgGfxAnim : public WgAnim
{
public:
	static WgGfxAnimPtr	Create() { return WgGfxAnimPtr(new WgGfxAnim()); }

	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgGfxAnimPtr	Cast( const WgObjectPtr& pObject );

	void		SetSize( WgSize size );
	WgSize		Size() const { return m_size; }

	bool		InsertFrame( int pos, const WgSurfacePtr& pSurf, WgCoord ofs, int duration );
	bool		InsertFrame( WgGfxFrame * pBefore, const WgSurfacePtr& pSurf, WgCoord ofs, int duration );
	bool		AddFrame( const WgSurfacePtr& pSurf, WgCoord ofs, int duration );
	int			AddFrames( const WgSurfacePtr& pSurf, WgCoord arrayOfs, WgSize arraySize, int duration, int nFrames = 0, WgSize spacing = WgSize() );
	int			AddFrames(const WgSurfacePtr& pSurf, int duration, int nFrames = 0, WgSize spacing = WgSize() );

	WgGfxFrame * GetFrame( int64_t ticks, WgGfxFrame * pProximity = 0 ) const;

	WgGfxFrame * GetFirstFrame(void) {return (WgGfxFrame *) WgAnim::_firstKeyFrame(); };
	WgGfxFrame * GetLastFrame(void) {return (WgGfxFrame *) WgAnim::_lastKeyFrame(); };

protected:
	WgGfxAnim();
	WgGfxAnim( WgSize size );

	WgSize		m_size;
};

//____ Class WgGfxFrame _______________________________________________________

class WgGfxFrame : public WgKeyFrame
{
public:
	// Derived from WgKeyFrame: Uint32	timestamp

	WgGfxFrame * GetNext(void) {return (WgGfxFrame *) WgKeyFrame::Next();};
	WgGfxFrame * GetPrev(void) {return (WgGfxFrame *) WgKeyFrame::Prev();};

	WgSurfacePtr	pSurf;
	WgRect			rect;
};

#endif //WG_GFXANIM_DOT_H
