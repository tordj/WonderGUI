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

#ifndef	WG_STREAMSINK_DOT_H
#define	WG_STREAMSINK_DOT_H
#pragma once

#include <wg_pointers.h>
#include <wg_gfxstream.h>



namespace wg
{

	class StreamSink;
	typedef	ComponentPtr<StreamSink>	StreamSink_p;


	//____ StreamSink __________________________________________________________

	class StreamSink : public GfxStream
	{
	public:

		//____ Holder ___________________________________________________

		class Holder /** @private */
		{
		public:

			virtual void	_processStreamChunks(const uint8_t* pBegin, const uint8_t* pEnd) = 0;
		};

		StreamSink(Holder * pHolder) : m_pHolder(pHolder) {}

		//.____ Control _______________________________________________________

		inline void	processChunks(const void* pBegin, const void* pEnd) { m_pHolder->_processStreamChunks( (uint8_t*) pBegin, (uint8_t*) pEnd); }

	protected:

		Holder * 	m_pHolder;
	};


} // namespace wg
#endif //WG_STREAMSINK_DOT_H
