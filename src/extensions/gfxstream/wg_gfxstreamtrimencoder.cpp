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

#include <wg_gfxstreamtrimencoder.h>

#include <algorithm>
#include <assert.h>

namespace wg
{
	const TypeInfo GfxStreamTrimEncoder::TYPEINFO = { "GfxStreamTrimEncoder", &Object::TYPEINFO };

	//____ constructor ________________________________________________________

	GfxStreamTrimEncoder::GfxStreamTrimEncoder(const GfxStreamSink_p& pStream, int bufferBytes)
	: GfxStreamEncoder(pStream)
	{
		m_activeScope = -1;
		m_outputBuffer.reserve(4096);		// Good starting value.
	}

	//____ destructor ________________________________________________________

	GfxStreamTrimEncoder::~GfxStreamTrimEncoder()
	{
	}


	//____ typeInfo() _________________________________________________________

	const TypeInfo& GfxStreamTrimEncoder::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ flush() ____________________________________________________________

	void GfxStreamTrimEncoder::flush()
	{
		if (m_pStream)
			m_pStream->processChunks( m_outputBuffer.data(), m_outputBuffer.data() + m_outputBuffer.size() );

		m_outputBuffer.clear();
	}

	//____ _beginChunk() _________________________________________________________

	void GfxStreamTrimEncoder::_beginChunk(GfxStream::Header header)
	{
		switch( header.type )
		{
			case GfxChunkId::BeginRender:
			{
				m_scopes.emplace_back(m_activeScope);
				m_activeScope = m_scopes.size() -1;

				m_scopes[m_activeScope].chunks.emplace_back( GfxChunkId::BeginRender, (int) m_data.size(), 0, GfxStream::SpxFormat::Int32_dec, 0 );
				break;
			}

			case GfxChunkId::EndRender:
			{
				m_scopes[m_activeScope].chunks.emplace_back(GfxChunkId::EndRender, (int) m_data.size(), 0, GfxStream::SpxFormat::Int32_dec, 0);
				_processScopes();
				flush();

				m_scopes.clear();
				m_activeScope = -1;
				break;
			}

			case GfxChunkId::BeginCanvasUpdate:
			{
				int newScopeIdx = m_scopes.size();
				int dataOfs = (int) m_data.size();

				// Signal jump to subscope in our scope with a special chunk.
				
				m_scopes[m_activeScope].chunks.emplace_back(GfxChunkId(-1), dataOfs, 0, GfxStream::SpxFormat::Int32_dec, newScopeIdx);

				// Create/setup our subscope
				
				m_scopes.push_back( Scope(m_activeScope) );
				m_activeScope = newScopeIdx;
				
				// Put BeginCanvasUpdate-chunk inside our subscope.
				
				m_scopes[m_activeScope].chunks.emplace_back(GfxChunkId::BeginCanvasUpdate, dataOfs, header.size, GfxStream::SpxFormat::Int32_dec, 0 );

				// Prepare data buffer for writes.

				m_data.resize(dataOfs+header.size);
				m_pWriteData = m_data.data() + dataOfs;
				break;
			}

			case GfxChunkId::EndCanvasUpdate:
			{
				m_scopes[m_activeScope].chunks.emplace_back(GfxChunkId::EndCanvasUpdate, (int) m_data.size(), 0, GfxStream::SpxFormat::Int32_dec, 0);
				m_activeScope = m_scopes[m_activeScope].prevScope;
				break;
			}

			default:
			{
				if( m_activeScope == -1 )
				{
					// Chunk is outside begin/end render scope. Just put it in outputBuffer directly.
					
					int ofs = (int) m_outputBuffer.size();
					if (header.size <= 30)
					{
						m_outputBuffer.resize(ofs + header.size + 2);
						m_pWriteData = m_outputBuffer.data() + ofs;

						_pushChar((char)header.type);
						_pushChar(header.size + (int(header.spxFormat) << 5));
					}
					else
					{
						m_outputBuffer.resize(ofs + header.size + 4);
						m_pWriteData = m_outputBuffer.data() + ofs;

						_pushChar((char)header.type);
						_pushChar(31 + (int(header.spxFormat) << 5));
						_pushShort((short)header.size);
					}
				}
				else
				{
					// Put header in our chunks
					
					int dataOfs = (int) m_data.size();
					m_scopes[m_activeScope].chunks.emplace_back(header.type, dataOfs, header.size, header.spxFormat, 0 );
				
					// Prepare data buffer for writes.
					
					m_data.resize(dataOfs+header.size);
					m_pWriteData = m_data.data() + dataOfs;
				}
				break;
			}
		}
	}

	//____ _processScopes() ______________________________________________________

	void GfxStreamTrimEncoder::_processScopes()
	{
		if( m_scopes.size() < 2 )
			return;
		
		
		for( auto it = m_scopes.begin() + 1 ; it != m_scopes.end() ; it++ )
			_optimizeClipLists( &(*it) );
		
		_writeScopeToOutput( &m_scopes[0] );
		
	}

	//____ _writeScopeToOutput() _________________________________________________

	void GfxStreamTrimEncoder::_writeScopeToOutput( Scope * pScope )
	{
		for( auto& chunk : pScope->chunks )
		{
			if( chunk.type == GfxChunkId::OutOfData )
				continue;									// This chunk has been optimized away
			
			if( chunk.type == GfxChunkId(-1) )
			{
				_writeScopeToOutput( &m_scopes[chunk.extra] );
			}
			else
			{
				if( chunk.type == GfxChunkId::DrawLineStraight )
				{
					int dummy = 0;
				}
				
				int ofs = m_outputBuffer.size();
				if (chunk.dataSize <= 30)
				{
					m_outputBuffer.resize(ofs + chunk.dataSize + 2);
					m_pWriteData = m_outputBuffer.data() + ofs;

					_pushChar((char)chunk.type);
					_pushChar(chunk.dataSize | (int(chunk.spxFormat) << 5));
				}
				else
				{
					m_outputBuffer.resize(ofs + chunk.dataSize + 4);
					m_pWriteData = m_outputBuffer.data() + ofs;

					_pushChar((char)chunk.type);
					_pushChar(31 | (int(chunk.spxFormat) << 5) );
					_pushShort((short)chunk.dataSize);
				}
				
				_pushBytes(chunk.dataSize, (char*) m_data.data() + chunk.dataOfs );
			}
		}
	}

	//____ _optimizeClipLists() ______________________________________________

	void GfxStreamTrimEncoder::_optimizeClipLists( Scope * pScope )
	{
		auto pChunk = pScope->chunks.data();
		auto pEnd = pChunk + pScope->chunks.size();

		assert( pChunk->type == GfxChunkId::BeginCanvasUpdate );
		
		int nCanvasRects = (pChunk->dataSize-4) / 16;
		RectSPX* pCanvasRects = (RectSPX*) (m_data.data() + pChunk->dataOfs + 4);

		
		int 		nActiveRects = nCanvasRects;
		RectSPX* 	pActiveRects = pCanvasRects;
		
		while( pChunk != pEnd )
		{
			
			switch( pChunk->type )
			{
				case GfxChunkId::SetClipList:
				{
					int nMyRects = pChunk->dataSize / 16;
					RectSPX* pMyRects = (RectSPX*)(m_data.data() + pChunk->dataOfs);

					if( _processSetClipList( pChunk, pActiveRects, nActiveRects, pMyRects, nMyRects) )
					{
						nActiveRects = pChunk->dataSize / 16;
						pActiveRects = (RectSPX*) (m_data.data() + pChunk->dataOfs);
					}
				}
					break;

				case GfxChunkId::PushClipList:
				{
					auto res = _processClipListScope( pChunk, pActiveRects, nActiveRects);
					pChunk = res.pNextChunk-1;
					break;
				}
					
				case GfxChunkId::ResetClipList:
				{
					if ( _processSetClipList(pChunk, pActiveRects, nActiveRects, pCanvasRects, nCanvasRects) )
					{
						nActiveRects = nCanvasRects;
						pActiveRects = pCanvasRects;
					}
					break;
				}
					
				default:
					break;
			}

			pChunk++;
		}

	}

	//____ _processClipListScope() _______________________________________________

	GfxStreamTrimEncoder::ClipListScopeResult GfxStreamTrimEncoder::_processClipListScope( ChunkInfo * pScope, RectSPX * pClipRects, int nClipRects )
	{
		bool bHasSetClipList = false;
		bool bDoesRendering = false;
		bool bSameClipList = false;
		
		assert( pScope->type == GfxChunkId::PushClipList );

		// Check if new scope has same clip list as old scope
		
		int nScopeRects = pScope->dataSize / 16;
		RectSPX* pScopeRects = (RectSPX*) (m_data.data() + pScope->dataOfs);
		
		bSameClipList = _compareClipLists(pClipRects, nClipRects, pScopeRects, nScopeRects );
		
		//
		
		int nActiveRects = nScopeRects;
		RectSPX * pActiveRects = pScopeRects;
			
		ChunkInfo * pChunk = pScope + 1;

		while( pChunk->type != GfxChunkId::PopClipList )
		{
			switch( pChunk->type )
			{
				case GfxChunkId::SetClipList:
				{
					int nMyRects = pChunk->dataSize / 16;
					RectSPX* pMyRects = (RectSPX*)(m_data.data() + pChunk->dataOfs);

					if( _processSetClipList(pChunk, pActiveRects, nActiveRects, pMyRects, nMyRects) )
					{
						bHasSetClipList = true;
						nActiveRects = pChunk->dataSize / 16;
						pActiveRects = (RectSPX*) (m_data.data() + pChunk->dataOfs);
					}
					break;
				}
					
				case GfxChunkId::ResetClipList:
				{
					if (_processSetClipList(pChunk, pActiveRects, nActiveRects, pScopeRects, nScopeRects))
					{
						bHasSetClipList = true;
						nActiveRects = nScopeRects;
						pActiveRects = pScopeRects;
					}
					break;
				}
					
				case GfxChunkId::PushClipList:
				{
					ClipListScopeResult res = _processClipListScope( pChunk, pActiveRects, nActiveRects );
					
					if( res.bAddsRendering )
						bDoesRendering = true;
					
					pChunk = res.pNextChunk -1;
					break;
				}
					
				default:
					if( !bDoesRendering && _isRenderingChunk(pChunk->type) )
						bDoesRendering = true;
					break;
			}
			
			pChunk++;
		}

		// Remove scope if we can and return.
		
		if( !bHasSetClipList && (bSameClipList || !bDoesRendering) )
		{
			pScope->type = GfxChunkId::OutOfData;
			pChunk->type = GfxChunkId::OutOfData;
			
			return { bDoesRendering, pChunk+1 };
		}
		else
			return { false, pChunk+1 };
		
		
	}

	//____ _isRenderingChunk() ___________________________________________________

	bool GfxStreamTrimEncoder::_isRenderingChunk(GfxChunkId chunkType)
	{
		int chunkId = int(chunkType);
		
		if( chunkId >= 17 && chunkId <= 44 && chunkId != 19 )
			return true;
		
		return false;
	}

	//____ _compareClipLists() ___________________________________________________

	bool GfxStreamTrimEncoder::_compareClipLists(RectSPX * pList1, int sizeList1, RectSPX * pList2, int sizeList2 )
	{
		if( sizeList1 != sizeList2 )
			return false;
			
		for( int i = 0 ; i < sizeList1 ; i++ )
		{
			if( pList1[i] != pList2[i] )
				return false;
		}

		return true;
	}

	//____ _processSetClipList() ________________________________________________

	bool GfxStreamTrimEncoder::_processSetClipList( ChunkInfo * pScope, RectSPX * pCurrRects, int nCurrRects, RectSPX * pMyRects, int nMyRects)
	{
		assert( pScope->type == GfxChunkId::SetClipList || pScope->type == GfxChunkId::ResetClipList );
		
		// Check if identical to current clip-list.
		
		if( _compareClipLists( pCurrRects, nCurrRects, pMyRects, nMyRects ) )
		{
			pScope->type = GfxChunkId::OutOfData;
			return false;
		}

		auto pChunk = pScope;

		while( true )
		{
			pChunk++;
			switch( pChunk->type )
			{
				case GfxChunkId::SetClipList:
				case GfxChunkId::ResetClipList:
				case GfxChunkId::PopClipList:
				case GfxChunkId::EndCanvasUpdate:
				{
					pScope->type = GfxChunkId::OutOfData;
					return false;							// We ran into end of update or change of cliplist before rendering anything.
				}

				case GfxChunkId::PushClipList:
				{
					int recursion = 1;
					while( recursion > 0 )
					{
						pChunk++;
						if( pChunk->type == GfxChunkId::PushClipList )
							recursion++;
						else if( pChunk->type == GfxChunkId::PopClipList )
							recursion--;
					}
				}
					
				default:
					if( _isRenderingChunk(pChunk->type) )
						return true;						// We are rendering with this cliplist so it needs to remain.
			}
		}
	}


}
