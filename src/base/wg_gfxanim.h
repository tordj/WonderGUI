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
#pragma once

#include <wg_anim.h>
#include <wg_types.h>
#include <wg_geo.h>
#include	<wg_surface.h>

namespace wg 
{
	
	class GfxFrame;
	
	class GfxAnim;
	typedef	StrongPtr<GfxAnim,Anim_p>		GfxAnim_p;
	typedef	WeakPtr<GfxAnim,Anim_wp>	GfxAnim_wp;
	
	//____ Class GfxAnim ________________________________________________________
	
	class	GfxAnim : public Anim
	{
	public:
		//.____ Creation __________________________________________

		static GfxAnim_p	create() { return GfxAnim_p(new GfxAnim()); }

		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static GfxAnim_p	cast( const Object_p& pObject );

		//.____ Misc ________________________________________________

		void		setSize( Size size );
		Size		size() const { return m_size; }
	
		bool		insertFrame( int pos, const Surface_p& pSurf, Coord ofs, int duration );
		bool		insertFrame( GfxFrame * pBefore, const Surface_p& pSurf, Coord ofs, int duration );
		bool		addFrame( const Surface_p& pSurf, Coord ofs, int duration );
		int			addFrames( const Surface_p& pSurf, Coord arrayOfs, Size arraySize, int duration, int nFrames = 0, Size spacing = Size() );
		int			addFrames(const Surface_p& pSurf, int duration, int nFrames = 0, Size spacing = Size() );
	
		GfxFrame * getFrame( int64_t ticks, GfxFrame * pProximity = 0 ) const;
	
		GfxFrame * getFirstFrame(void) {return (GfxFrame *) Anim::_firstKeyFrame(); };
		GfxFrame * getLastFrame(void) {return (GfxFrame *) Anim::_lastKeyFrame(); };
	
	protected:
		GfxAnim();
		GfxAnim( Size size );
	
		Size		m_size;
	};
	
	//____ Class GfxFrame _______________________________________________________
	
	class GfxFrame : public KeyFrame
	{
	public:
		// Derived from KeyFrame: uint32_t	timestamp
	
		GfxFrame * getNext(void) {return (GfxFrame *) KeyFrame::next();};
		GfxFrame * getPrev(void) {return (GfxFrame *) KeyFrame::prev();};
	
		Surface_p	pSurf;
		Rect			rect;
	};
	

} // namespace wg
#endif //WG_GFXANIM_DOT_H
