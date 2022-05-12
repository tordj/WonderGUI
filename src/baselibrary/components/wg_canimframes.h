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
#ifndef WG_CANIMFRAMES_DOT_H
#define WG_CANIMFRAMES_DOT_H
#pragma once


#include <wg_cdynamicvector.h>
#include <wg_surface.h>
#include <wg_geo.h>
#include <wg_cstaticvector.h>
#include <wg_cstaticvector.impl.h>

namespace wg
{

	class AnimFrame
	{
		friend class CAnimFrames;
	public:
		AnimFrame(Coord source, int duration, GfxFlip flip = GfxFlip::Normal) : m_source(source), m_timestamp(0), m_duration(duration), m_flip(flip) {}

		int		duration() const { return m_duration; }
		GfxFlip	flip() const { return m_flip; }
		Coord	source() const { return m_source; }
		int		timestamp() const { return m_timestamp; }

	protected:
		Coord	m_source;		// Points
		int		m_timestamp;	// Millisec
		int		m_duration;		// Millisec
		GfxFlip	m_flip;			

	};


	//____ CAnimFrames _____________________________________________________
	/**
		@brief Vector of frames for a bitmap-based animation.

		The CAnimFrames component contains a vector of rectangular bitmap
		areas, all of the same size, used as frames in an animation.

		All frames must be from the same Surface. Each frame
		has an individual duration (set in milliseconds) so the frame rate
		can vary throughout the animation. Frames can overlap each other and
		each frame can also be flipped or rotated in 90 degrees increments, which
		can be used to cut down on the amount of graphics needed for certain animations.
	*/

	class CAnimFrames : public CDynamicVector<AnimFrame>
	{
	public:
		 
		class Holder : public CDynamicVector<AnimFrame>::Holder
		{
		public:
			virtual void	_didSetAnimFrameSize(CAnimFrames * pComponent) = 0;
			virtual void	_didSetAnimSurface(CAnimFrames * pComponent) = 0;
		};

		CAnimFrames(Holder * pHolder) : CDynamicVector(pHolder) {}

		
		//.____ Identification _________________________________________________

		const TypeInfo& typeInfo(void) const override { return TYPEINFO; }
		const static TypeInfo	TYPEINFO;

		//.____ Content _______________________________________________________

		void			setSurface(Surface* pSurface);
		Surface_p		surface() const { return m_pSurface; }

		void			setFrameSize(Size pointSize);
		Size			frameSize() const { return m_frameSize; }

		inline int		duration() const;
		const AnimFrame * find(int timestamp) const;

		//.____ Internal ______________________________________________________

		Surface *		_surface() const { return m_pSurface; }


	protected:
		//		~CVector() {}

		void	_didAddEntries(AnimFrame * pEntry, int nb) override;
		void	_didMoveEntries(AnimFrame * pFrom, AnimFrame * pTo, int nb) override;
		void	_willEraseEntries(AnimFrame * pEntry, int nb) override;


		int			m_duration = 0;
		Size		m_frameSize;
		Surface_p	m_pSurface;
	};

	//____ duration() _________________________________________________________
	/**
		@brief Return the duration for the entire animation.

		Return the duration for the entire animation, measured in milliseconds.

		@return Duration of the animation.
	*/

	inline int CAnimFrames::duration() const 
	{ 
		return m_duration; 
	}



} //namespace wg

#endif //WG_CANIMFRAMES_DOT_H
