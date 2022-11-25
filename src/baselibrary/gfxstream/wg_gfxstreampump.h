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
#include <wg_componentptr.h>

#include <wg_gfxstreamsource.h>
#include <wg_gfxstreamsink.h>

#include <vector>

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
		bool		pumpAllFramesWithOptimization();
		bool		pumpAll();


	protected:

		GfxStreamPump(const GfxStreamSource_p& pInput, const GfxStreamSink_p& pOutput);
		~GfxStreamPump();

		const uint8_t*	_findChunk(GfxChunkId id, const uint8_t* pBegin, const uint8_t* pEnd);
		void			_maskAddRect(std::vector<RectI>& vRects, int startOffset, const RectI& rect);


		bool			_pumpUntilChunk(GfxChunkId id, bool bInclusive );
		bool			_fetchUntilChunk(GfxChunkId id);


		GfxStreamSource_p	m_pInput;
		GfxStreamSink_p	m_pOutput;
	};
}

#endif // WG_GFXSTREAMPUMP_DOT_H
