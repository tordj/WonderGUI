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

#ifndef	WG_GFXSTREAMSINK_DOT_H
#define	WG_GFXSTREAMSINK_DOT_H
#pragma once

#include <wg_pointers.h>
#include <wg_gfxstream.h>



namespace wg
{

	class GfxStreamSink;
	typedef	ComponentPtr<GfxStreamSink>	GfxStreamSink_p;


	//____ GfxStreamSink __________________________________________________________

	class GfxStreamSink : public GfxStream
	{
	public:

		//____ Holder ___________________________________________________

		class Holder /** @private */
		{
		public:

			virtual void	_processStreamChunks(const uint8_t* pBegin, const uint8_t* pEnd) = 0;
		};

		GfxStreamSink(Holder * pHolder) : m_pHolder(pHolder) {}

		//.____ Control _______________________________________________________

		inline void	processChunks(const uint8_t* pBegin, const uint8_t* pEnd) { m_pHolder->_processStreamChunks(pBegin, pEnd); }

	protected:

		Holder * 	m_pHolder;
	};


} // namespace wg
#endif //WG_GFXSTREAMSINK_DOT_H
