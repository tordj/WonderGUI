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

namespace wg
{

	class AnimFrame
	{
		friend class CAnimFrames;
	public:
		AnimFrame(CoordI source, int duration) : m_timestamp(0), m_duration(duration), m_source(source) {}

		int		timestamp() const { return m_timestamp; }
		int		duration() const { return m_duration; }
		CoordI	source() const { return m_source; }

	protected:
		int		m_timestamp;	// Millisec
		int		m_duration;		// Millisec
		CoordI	m_source;		// Points
	};


	//____ CAnimFrames _____________________________________________________

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

		void			setFrameSize(SizeI pointSize);
		SizeI			frameSize() const { return m_frameSize; }

		inline int		duration() const { return m_duration; }
		const AnimFrame * find(int timestamp) const;

		//.____ Internal ______________________________________________________

		Surface *		_surface() const { return m_pSurface; }


	protected:
		//		~CVector() {}

		void	_didAddEntries(AnimFrame * pEntry, int nb) override;
		void	_didMoveEntries(AnimFrame * pFrom, AnimFrame * pTo, int nb) override;
		void	_willEraseEntries(AnimFrame * pEntry, int nb) override;


		int			m_duration = 0;
		SizeI		m_frameSize;
		Surface_p	m_pSurface;
	};


} //namespace wg

#endif //WG_CANIMFRAMES_DOT_H
