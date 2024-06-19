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

#ifndef	WG_STREAMFASTENCODER_DOT_H
#define	WG_STREAMFASTENCODER_DOT_H
#pragma once

#include <cstring>

#include <wg_streamsink.h>
#include <wg_componentptr.h>
#include <wg_gfxtypes.h>
#include <wg_geo.h>
#include <wg_color.h>
#include <wg_gradient.h>
#include <wg_streamencoder.h>

namespace wg
{
	class StreamFastEncoder;
	typedef	StrongPtr<StreamFastEncoder>	StreamFastEncoder_p;
	typedef	WeakPtr<StreamFastEncoder>		StreamFastEncoder_wp;

	//____ StreamFastEncoder ___________________________________________________

	class StreamFastEncoder : public StreamEncoder
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

		static StreamFastEncoder_p	create( const StreamSink_p& pStream, int bufferBytes = GfxStream::c_maxBlockSize*2 ) { return StreamFastEncoder_p(new StreamFastEncoder(pStream, bufferBytes)); }
		static StreamFastEncoder_p	create( const Blueprint& blueprint ) { return StreamFastEncoder_p(new StreamFastEncoder(blueprint)); }

		//.____ Identification __________________________________________

		const TypeInfo& typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Control _______________________________________________________

		void		flush() override;

	protected:
		StreamFastEncoder( const StreamSink_p& pStream, int bufferBytes );
		template<class BP> StreamFastEncoder( const BP& bp ) : StreamEncoder(bp)
		{
			m_pBuffer = new uint8_t[bp.bufferBytes];
			m_capacity = bp.bufferBytes;
			m_pWriteData = m_pBuffer;
		}
		
		~StreamFastEncoder();

		void _beginChunk(GfxStream::Header header) override;
		
		uint8_t*	m_pBuffer = nullptr;
		int			m_capacity = 0;
	};

}



#endif // WG_STREAMFASTENCODER_DOT_H
