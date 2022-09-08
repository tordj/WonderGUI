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
#ifndef WG3_GFXSTREAMPUMP_DOT_H
#define WG3_GFXSTREAMPUMP_DOT_H
#pragma once

#include <wg3_geo.h>
#include <wg3_object.h>
#include <wg3_componentptr.h>

#include <wg3_gfxstreamsource.h>
#include <wg3_gfxstreamsink.h>

namespace wg
{
	class GfxStreamPump;
	typedef	StrongPtr<GfxStreamPump>	GfxStreamPump_p;
	typedef	WeakPtr<GfxStreamPump>		GfxStreamPump_wp;

	class GfxStreamPump : public Object
	{
	public:

		//.____ Creation ______________________________________________________

		static GfxStreamPump_p	create();
		static GfxStreamPump_p	create( const GfxStreamSource_p& pInput, const GfxStreamSink_p& pOutput );

		//.____ Identification ________________________________________________

		const TypeInfo& typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Control _______________________________________________________

		void		setInput(const GfxStreamSource_p& pInput);
		void		setOutput(const GfxStreamSink_p& pOutput);

		GfxChunkId	peekChunk();
		bool		pumpChunk();
		bool		pumpUntilFrame();
		bool		pumpFrame();
//		bool		pumpAllFramesWithOptimization();
		bool		pumpAll();


	protected:

		GfxStreamPump(const GfxStreamSource_p& pInput, const GfxStreamSink_p& pOutput);
		~GfxStreamPump();

		inline GfxStream::Header _decodeHeader(const uint8_t* pHeader) const
		{
			return { * (GfxStream::Header *) pHeader };
		}

		const uint8_t* _findChunk(GfxChunkId id, int nSegments, const DataSegment * pSegments);

		bool _pumpUntilChunk(GfxChunkId id, bool bInclusive);


		GfxStreamSource_p	m_pInput;
		GfxStreamSink_p	m_pOutput;
	};
}

#endif // WG3_GFXSTREAMPUMP_DOT_H
