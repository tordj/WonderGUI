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
#include <wg_canimframes.h>
#include <wg_cdynamicvector.impl.h>

#include <algorithm>

namespace wg
{
	template class CDynamicVector<AnimFrame>;


	const TypeInfo CAnimFrames::TYPEINFO = { "CAnimFrames", &CStaticVector<AnimFrame>::TYPEINFO };


	//____ setSurface() __________________________________________________________

	void CAnimFrames::setSurface(Surface* pSurface)
	{
		if (pSurface != m_pSurface)
		{
			m_pSurface = pSurface;
			static_cast<CAnimFrames::Holder*>(m_pHolder)->_didSetAnimSurface(this);
		}
	}

	//____ setFrameSize() __________________________________________________________

	void CAnimFrames::setFrameSize(SizeI pointSize)
	{
		if (pointSize != m_frameSize)
		{
			m_frameSize = pointSize;
			static_cast<CAnimFrames::Holder*>(m_pHolder)->_didSetAnimFrameSize(this);
		}
	}

	//____ find() _____________________________________________________________
	/**
		@brief Find frame to display for specific timestamp

		This method returns a pointer to the frame to display for the given timestamp.

		Negative values will return the first frame and values beyond the last frame
		will return the last frame.

		@timestamp Timestamp in milliseconds.

		@return Pointer to the frame for the timestamp or nullptr if component contains
				no frames.
	*/
	const AnimFrame* CAnimFrames::find(int timestamp) const
	{
		if (m_entries.empty())
			return nullptr;

		const AnimFrame* pFirst = &m_entries.front();
		const AnimFrame* pLast = &m_entries.back();
		const AnimFrame* pFrame = pFirst + (pLast - pFirst);

		if (timestamp < pFirst->m_duration)
			return pFirst;

		if (timestamp >= pLast->m_timestamp)
			return pLast;

		while (true)
		{
			if (timestamp < pFrame->m_timestamp)
			{
				pLast = pFrame;
				pFrame -= (pFrame - pFirst +1) / 2;
			}
			else if (timestamp < pFrame->m_timestamp + pFrame->m_duration)
				return pFrame;
			else
			{
				pFirst = pFrame;
				pFrame += (pLast - pFrame +1) / 2;
			}
		}
	}

	//____ _didAddEntries() ___________________________________________________

	void CAnimFrames::_didAddEntries(AnimFrame* pEntry, int nb)
	{
		int ofs = int(pEntry - &m_entries.front());
		int timestamp = ofs == 0 ? 0 : pEntry[-1].timestamp() + pEntry[-1].duration();

		auto it = m_entries.begin() + ofs;

		for (int i = 0; i < nb; i++, it++)
		{
			it->m_timestamp = timestamp;
			timestamp += it->m_duration;
			m_duration += it->m_duration;
		}

		while (it != m_entries.end())
		{
			it->m_timestamp = timestamp;
			timestamp += it->m_duration;
		}

		CDynamicVector::_didAddEntries(pEntry, nb);
	}

	//____ _didMoveEntries() __________________________________________________

	void CAnimFrames::_didMoveEntries(AnimFrame* pFrom, AnimFrame* pTo, int nb)
	{
		AnimFrame* pEntry = std::min(pFrom, pTo);

		int ofs = int(pEntry - &m_entries.front());
		int timestamp = ofs == 0 ? 0 : pEntry[-1].timestamp() + pEntry[-1].duration();
		for (auto it = m_entries.begin() + ofs; it != m_entries.end(); it++)
		{
			it->m_timestamp = timestamp;
			timestamp += it->m_duration;
		}

		CDynamicVector::_didMoveEntries(pFrom, pTo, nb);
	}

	//____ _willEraseEntries() ___________________________________________________

	void CAnimFrames::_willEraseEntries(AnimFrame* pEntry, int nb)
	{
		int ofs = int(pEntry - &m_entries.front());
		int timestamp = pEntry->timestamp() + pEntry->duration();

		auto it = m_entries.begin() + ofs;

		for (int i = 0; i < nb; i++, it++)
			m_duration -= it->m_duration;

		while (it != m_entries.end())
		{
			it->m_timestamp = timestamp;
			timestamp += it->m_duration;
		}

		CDynamicVector::_willEraseEntries(pEntry, nb);
	}


} //namespace wg
