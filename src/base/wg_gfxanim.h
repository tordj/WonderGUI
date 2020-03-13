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
	typedef	StrongPtr<GfxAnim>		GfxAnim_p;
	typedef	WeakPtr<GfxAnim>	GfxAnim_wp;

	//____ Class GfxAnim ________________________________________________________

	class	GfxAnim : public Anim
	{
	public:
		//.____ Creation __________________________________________

		static GfxAnim_p	create() { return GfxAnim_p(new GfxAnim()); }

		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const override;
		const char *		className( void ) const override;
		static const char	CLASSNAME[];

		//.____ Misc ________________________________________________

		void		setSize( SizeI size );
		SizeI		size() const { return m_size; }

		bool		insertFrame( int pos, Surface * pSurf, CoordI ofs, int duration );
		bool		insertFrame( GfxFrame * pBefore, Surface * pSurf, CoordI ofs, int duration );
		bool		addFrame( Surface * pSurf, CoordI ofs, int duration );
		int			addFrames( Surface * pSurf, CoordI arrayOfs, SizeI arraySize, int duration, int nFrames = 0, SizeI spacing = SizeI() );
		int			addFrames(Surface * pSurf, int duration, int nFrames = 0, SizeI spacing = SizeI() );

		GfxFrame * getFrame( int64_t ticks, GfxFrame * pProximity = 0 ) const;

		GfxFrame * getFirstFrame(void) {return (GfxFrame *) Anim::_firstKeyFrame(); };
		GfxFrame * getLastFrame(void) {return (GfxFrame *) Anim::_lastKeyFrame(); };

	protected:
		GfxAnim();
		GfxAnim( SizeI size );

		SizeI		m_size;
	};

	//____ Class GfxFrame _______________________________________________________

	class GfxFrame : public KeyFrame
	{
	public:
		// Derived from KeyFrame: uint32_t	timestamp

		//.____ Misc ___________________________________________________________

		GfxFrame * getNext(void) {return (GfxFrame *) KeyFrame::next();};
		GfxFrame * getPrev(void) {return (GfxFrame *) KeyFrame::prev();};

		Surface_p	pSurf;
		RectI			rect;
	};


} // namespace wg
#endif //WG_GFXANIM_DOT_H
