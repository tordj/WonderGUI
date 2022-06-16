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
#ifndef WG_GFXSTREAMPUMP_DOT_H
#define WG_GFXSTREAMPUMP_DOT_H
#pragma once

#include <wg_geo.h>
#include <wg_object.h>

#include <wg_cgfxinstream.h>
#include <wg_cgfxoutstream.h>

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
		static GfxStreamPump_p	create( CGfxInStream * pInput, CGfxOutStream * pOutput );

		//.____ Identification ________________________________________________

		const TypeInfo& typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Control _______________________________________________________

		void		setInput(CGfxInStream* pStream);
		void		setOutput(CGfxOutStream* pStream);

		GfxChunkId	peekChunk();
		bool		pumpChunk();
		bool		pumpUntilFrame();
		bool		pumpFrame();
//		bool		pumpAllFramesWithOptimization();
		bool		pumpAll();


	protected:

		GfxStreamPump(CGfxInStream* pInput, CGfxOutStream* pOutput);
		~GfxStreamPump();

		inline GfxStream::Header _decodeHeader(const uint8_t* pHeader) const
		{
			return { * (GfxStream::Header *) pHeader };
		}

		const uint8_t* _findChunk(GfxChunkId id, int nSegments, const DataSegment * pSegments);

		bool _pumpUntilChunk(GfxChunkId id, bool bInclusive);


		CGfxInStream_p		m_pInput;
		CGfxOutStream_p		m_pOutput;
	};
}

#endif // WG_GFXSTREAMPUMP_DOT_H
