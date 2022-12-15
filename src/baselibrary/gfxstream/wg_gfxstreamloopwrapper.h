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

#ifndef	WG_GFXSTREAMLOOPWRAPPER_DOT_H
#define	WG_GFXSTREAMLOOPWRAPPER_DOT_H
#pragma once

#include <wg_types.h>
#include <wg_object.h>
#include <wg_pointers.h>
#include <wg_cgfxinstream.h>
#include <wg_gfxstream.h>

#include <functional>

namespace wg
{

	class GfxStreamLoopWrapper;
	typedef	StrongPtr<GfxStreamReader>	GfxStreamLoopWrapper_p;
	typedef	WeakPtr<GfxStreamReader>	GfxStreamLoopWrapper_wp;

	class GfxStreamLoopWrapper : public Object, protected CGfxInStream::Holder
	{
	public:

		//.____ Creation __________________________________________

		static GfxStreamLoopWrapper_p	create( const void * pBufferBegin, const void * pBufferEnd, 
												std::function<std::tuple<const void * pBegin, const void * pEnd>()> getContentFunc,
												std::function<void(const void * pReadPos)> setContentBeginFunc );

		//.____ Components _______________________________________

		CGfxInStream		output;

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Misc _____________________________________________________
		
	protected:

		GfxStreamReader(std::function<int(int nBytes, void * pDest)> dataFeeder );
		~GfxStreamReader();

		Object *    	_object() override { return this; }
		const Object * 	_object() const override { return this; }

		bool 			_hasStreamChunks() const override;
		std::tuple<int, const DataSegment*> _showStreamChunks() override;
		GfxChunkId 		_peekStreamChunk() override;
		void 			_discardStreamChunks(int bytes) override;
		bool 			_fetchStreamChunks() override;

		std::function<int(int nBytes, void * pDest)>	m_fetcher;

		const void * m_pBufferBegin; 
		const void * m_pBufferEnd;

		std::function<std::tuple<const void * pBegin, const void * pEnd>()> m_getContentFunc;
		std::function<void(const void * pReadPos)> 							m_setContentBeginFunc;

		char	m_rejoinedChunk[];


		DataSegment	m_dataSegments[3];

	};



} // namespace wg
#endif //WG_GFXSTREAMLOOPWRAPPER_DOT_H

