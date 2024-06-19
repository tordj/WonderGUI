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

#ifndef	WG_STREAMTRIMENCODER_DOT_H
#define	WG_STREAMTRIMENCODER_DOT_H
#pragma once

#include <cstring>
#include <vector>

#include <wg_streamsink.h>
#include <wg_componentptr.h>
#include <wg_gfxtypes.h>
#include <wg_geo.h>
#include <wg_color.h>
#include <wg_gradient.h>
#include <wg_streamencoder.h>

namespace wg
{
	class StreamTrimEncoder;
	typedef	StrongPtr<StreamTrimEncoder>	StreamTrimEncoder_p;
	typedef	WeakPtr<StreamTrimEncoder>	StreamTrimEncoder_wp;

	//____ StreamTrimEncoder ___________________________________________________

	class StreamTrimEncoder : public StreamEncoder
	{
	public:


		//.____ Blueprint __________________________________________________

		struct Blueprint
		{
			int				bufferBytes		= GfxStream::c_maxBlockSize*2;
			Finalizer_p		finalizer;
			uint16_t		objectIdStart	= 0;
			PixelFormat		pixelFormat		= PixelFormat::BGRA_8;
			SampleMethod	sampleMethod	= SampleMethod::Nearest;
			StreamSink_p	sink;
		};
		
		//.____ Creation __________________________________________

		static StreamTrimEncoder_p	create( const StreamSink_p& pStream, int bufferBytes = GfxStream::c_maxBlockSize*2 ) { return StreamTrimEncoder_p(new StreamTrimEncoder(pStream, bufferBytes)); }
		static StreamTrimEncoder_p	create( const Blueprint& blueprint ) { return StreamTrimEncoder_p(new StreamTrimEncoder(blueprint)); }

		//.____ Identification __________________________________________

		const TypeInfo& typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Control _______________________________________________________

		void		flush() override;

	protected:
		StreamTrimEncoder( const StreamSink_p& pStream, int bufferBytes );
		template<class BP> StreamTrimEncoder( const BP& bp ) : StreamEncoder(bp)
		{
			m_outputBuffer.reserve(4096);		// Good starting value.
		}
		~StreamTrimEncoder();

		void _beginChunk(GfxStream::Header header) override;
		
		struct ChunkInfo
		{
			ChunkInfo(GfxChunkId type, int dataOfs, int dataSize, GfxStream::SpxFormat spxFormat, int extra )
			: type(type), dataOfs(dataOfs), dataSize(dataSize), spxFormat(spxFormat), extra(extra) {}
			
			
			GfxChunkId	type;
			int 		dataOfs;		// Offset into data-vector for this chunks data.
			int			dataSize;
			GfxStream::SpxFormat	spxFormat;
			int			extra;			// Additional chunk-specific info. Used by BeginCanvasUpdate to number new scope.
		};
		
		struct Scope
		{
			Scope(int prevScope) : prevScope(prevScope) {}
			
			int 					prevScope = -1;
			std::vector<ChunkInfo>	chunks;
		};

		void	_processScopes();
		void	_writeScopeToOutput( Scope * pScope );
		void	_optimizeClipLists( Scope * pScope );


		bool	_compareClipLists(RectSPX * pList1, int sizeList1, RectSPX * pList2, int sizeList2 );
		bool	_isRenderingChunk(GfxChunkId chunkType);

		struct ClipListScopeResult
		{
			bool	bAddsRendering;
			ChunkInfo * pNextChunk;
		};
		
		ClipListScopeResult	_processClipListScope( ChunkInfo * pNextChunk, RectSPX * pClipRects, int nClipRects );
		bool 				_processSetClipList( ChunkInfo * pSetClipListChunk, RectSPX * pActiveRects, int nActiveRects, RectSPX * pMyRects, int nMyRects );

		
		int						m_activeScope = -1;
		
		std::vector<Scope>		m_scopes;
		
		std::vector<uint8_t>	m_data;					// Data for chunks in scopes
		
		std::vector<uint8_t>	m_outputBuffer;			// Optimized stream, ready to be flushed.
		
	};



}






#endif // WG_STREAMENCODER_DOT_H
