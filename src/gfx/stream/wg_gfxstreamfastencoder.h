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

#ifndef	WG_GFXSTREAMFASTENCODER_DOT_H
#define	WG_GFXSTREAMFASTENCODER_DOT_H
#pragma once

#include <cstring>

#include <wg_gfxstreamsink.h>
#include <wg_componentptr.h>
#include <wg_gfxtypes.h>
#include <wg_geo.h>
#include <wg_color.h>
#include <wg_gradient.h>
#include <wg_gfxstreamencoder.h>

namespace wg
{
	class GfxStreamFastEncoder;
	typedef	StrongPtr<GfxStreamFastEncoder>	GfxStreamFastEncoder_p;
	typedef	WeakPtr<GfxStreamFastEncoder>	GfxStreamFastEncoder_wp;

	//____ GfxStreamFastEncoder ___________________________________________________

	class GfxStreamFastEncoder : public GfxStreamEncoder
	{
	public:

		//.____ Creation __________________________________________

		static GfxStreamFastEncoder_p	create( const GfxStreamSink_p& pStream, int bufferBytes = GfxStream::c_maxBlockSize*2 ) { return GfxStreamFastEncoder_p(new GfxStreamFastEncoder(pStream, bufferBytes)); }

		//.____ Identification __________________________________________

		const TypeInfo& typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Control _______________________________________________________

		void		flush() override;

		//.____ Operators _____________________________________________________

		GfxStreamEncoder& operator<< (GfxStream::Header) override;

	protected:
		GfxStreamFastEncoder( const GfxStreamSink_p& pStream, int bufferBytes );
		~GfxStreamFastEncoder();

		uint8_t*	m_pBuffer = nullptr;
		int			m_capacity = 0;
	};

}



#endif // WG_GFXSTREAMFASTENCODER_DOT_H
