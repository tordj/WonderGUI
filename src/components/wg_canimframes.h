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

	class CAnimFrames_Frame;

	//____ CAnimFrames _____________________________________________________

	class CAnimFrames : public CDynamicVector<CAnimFrames_Frame>
	{
	public:

		class Frame
		{
			friend class CAnimFrames;

			Frame(CoordI source, int duration);

			int		timestamp() const { return m_timestamp; }
			int		duration() const { return m_duration; }
			CoordI	source() const { return m_source; }

		protected:
			int		m_timestamp;	// Millisec
			int		m_duration;		// Millisec
			CoordI	m_source;		// Points
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
		const Frame&	find(int timestamp) const;

	protected:
		//		~CVector() {}

		void	_didAddEntries(CAnimFrames_Frame * pEntry, int nb) override;
		void	_didMoveEntries(CAnimFrames_Frame * pFrom, CAnimFrames_Frame * pTo, int nb) override;
		void	_willEraseEntries(CAnimFrames_Frame * pEntry, int nb) override;


		int			m_duration = 0;
		SizeI		m_frameSize;
		Surface_p	m_pSurface;
	};

	class CAnimFrames_Frame : public CAnimFrames::Frame /** @private */ {};	// Workaround for C++ not allowing forward declaration of CAnimFrames::Frame


} //namespace wg

#endif //WG_CANIMFRAMES_DOT_H