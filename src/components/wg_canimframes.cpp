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

#include <algorithm>

namespace wg
{

	const TypeInfo CAnimFrames::TYPEINFO = { "CAnimFrames", &CStaticVector<CAnimFrames::Frame>::TYPEINFO };


	//____ setSurface() __________________________________________________________

	void CAnimFrames::setSurface(Surface* pSurface)
	{
		m_pSurface = pSurface;
	}

	//____ setFrameSize() __________________________________________________________

	void CAnimFrames::setFrameSize(SizeI pointSize)
	{
		m_frameSize = pointSize;
	}

	//____ find() _____________________________________________________________

	const CAnimFrames::Frame& CAnimFrames::find(int timestamp) const
	{
		const Frame* pFirst = &m_entries.front();
		const Frame* pLast = &m_entries.back();
		const Frame* pFrame = pFirst + (pLast - pFirst);

		if (timestamp < pFirst->m_duration)
			return *pFirst;

		if (timestamp >= pLast->m_timestamp)
			return *pLast;

		while (true)
		{
			if (timestamp < pFrame->m_timestamp)
				pFrame -= (pFrame - pFirst) / 2;
			else if (timestamp < pFrame->m_timestamp + pFrame->m_duration)
				return *pFrame;
			else
				pFrame += (pLast - pFrame) / 2;
		}
	}

	//____ _didAddEntries() ___________________________________________________

	void CAnimFrames::_didAddEntries(CAnimFrames_Frame* pEntry, int nb)
	{
		int ofs = pEntry - &m_entries.front();
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

	void CAnimFrames::_didMoveEntries(CAnimFrames_Frame* pFrom, CAnimFrames_Frame* pTo, int nb)
	{
		CAnimFrames_Frame* pEntry = std::min(pFrom, pTo);

		int ofs = pEntry - &m_entries.front();
		int timestamp = ofs == 0 ? 0 : pEntry[-1].timestamp() + pEntry[-1].duration();
		for (auto it = m_entries.begin() + ofs; it != m_entries.end(); it++)
		{
			it->m_timestamp = timestamp;
			timestamp += it->m_duration;
		}

		CDynamicVector::_didMoveEntries(pFrom, pTo, nb);
	}

	//____ _willEraseEntries() ___________________________________________________

	void CAnimFrames::_willEraseEntries(CAnimFrames_Frame* pEntry, int nb)
	{
		int ofs = pEntry - &m_entries.front();
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