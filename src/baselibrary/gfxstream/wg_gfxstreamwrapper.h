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

#ifndef	WG_GFXSTREAMWRAPPER_DOT_H
#define	WG_GFXSTREAMWRAPPER_DOT_H
#pragma once

#include <wg_types.h>
#include <wg_object.h>
#include <wg_pointers.h>
#include <wg_gfxstreamsource.h>
#include <wg_gfxstream.h>

#include <functional>

namespace wg
{

	class GfxStreamWrapper;
	typedef	StrongPtr<GfxStreamWrapper>	GfxStreamWrapper_p;
	typedef	WeakPtr<GfxStreamWrapper>	GfxStreamWrapper_wp;

	class GfxStreamWrapper : public Object, protected GfxStreamSource::Holder
	{
	public:

		//.____ Creation __________________________________________

		static GfxStreamWrapper_p	create( const void * pBegin, const void * pEnd );

		//.____ Components _______________________________________

		GfxStreamSource		output;

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Misc _____________________________________________________
		
		inline bool	chunkInBuffer() const { return _hasStreamChunks(); };
		int			bytesInBuffer() const;

		void		restart() { m_dataSegment.pBegin = m_pBegin; }

	protected:

		GfxStreamWrapper( const void * pBegin, const void * pEnd );
		~GfxStreamWrapper();

		bool 			_hasStreamChunks() const override;
		std::tuple<int, const DataSegment*> _showStreamChunks() override;
		GfxChunkId 		_peekStreamChunk() override;
		void 			_discardStreamChunks(int bytes) override;
		bool 			_fetchStreamChunks() override;

	
		uint8_t *	m_pBegin;
		uint8_t *	m_pEnd;

		DataSegment	m_dataSegment;
	};



} // namespace wg
#endif //WG_GFXSTREAMWRAPPER_DOT_H
#pragma once
