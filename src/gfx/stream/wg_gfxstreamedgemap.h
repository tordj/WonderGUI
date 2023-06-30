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

#ifndef WG_GFXSTREAMEDGEMAP_DOT_H
#define WG_GFXSTREAMEDGEMAP_DOT_H
#pragma once

#include <wg_edgemap.h>
#include <wg_gfxstreamencoder.h>

namespace wg
{

	class GfxStreamEdgemap;
	typedef StrongPtr<GfxStreamEdgemap> GfxStreamEdgemap_p;
	typedef WeakPtr<GfxStreamEdgemap> GfxStreamEdgemap_wp;

	class GfxStreamEdgemap : public Edgemap
	{
		friend class GfxStreamDevice;
	public:

		//.____ Creation __________________________________________
		
		static GfxStreamEdgemap_p	create( GfxStreamEncoder * pEncoder, const Blueprint& blueprint );
		static GfxStreamEdgemap_p	create( GfxStreamEncoder * pEncoder, const Edgemap::Blueprint& blueprint, WaveOrigo origo, const float * pSamples, int edges, int edgePitch = 0, int samplePitch = 0);
		static GfxStreamEdgemap_p	create( GfxStreamEncoder * pEncoder, const Edgemap::Blueprint& blueprint, WaveOrigo origo, const spx * pSamples, int edges, int edgePitch = 0, int samplePitch = 0);


    	//.____ Identification __________________________________________

		const TypeInfo&     typeInfo(void) const override;
		const static TypeInfo   TYPEINFO;

		//.____ Appearance ____________________________________________________

		bool	setRenderSegments(int nSegments) override;

		//.____ Content _______________________________________________________

		bool 	importSamples( WaveOrigo origo, const spx * pSource, int edgeBegin, int edgeEnd,
									  int sampleBegin, int sampleEnd, int edgePitch = 0, int samplePitch = 0 ) override;

		bool 	importSamples( WaveOrigo origo, const float * pSource, int edgeBegin, int edgeEnd,
									  int sampleBegin, int sampleEnd, int edgePitch = 0, int samplePitch = 0 ) override;

		bool 	exportSamples( WaveOrigo origo, spx * pDestination, int edgeBegin, int edgeEnd,
									  int sampleBegin, int sampleEnd, int edgePitch = 0, int samplePitch = 0 ) override;

		bool 	exportSamples( WaveOrigo origo, float * pDestination, int edgeBegin, int edgeEnd,
									  int sampleBegin, int sampleEnd, int edgePitch = 0, int samplePitch = 0 ) override;

	protected:

		GfxStreamEdgemap( GfxStreamEncoder * pEncoder, const Blueprint& bp );
		~GfxStreamEdgemap();

		void 	_importSamples( WaveOrigo origo, const spx * pSource, int edgeBegin, int edgeEnd,
								int sampleBegin, int sampleEnd, int edgePitch, int samplePitch );

		void 	_importSamples( WaveOrigo origo, const float * pSource, int edgeBegin, int edgeEnd,
								int sampleBegin, int sampleEnd, int edgePitch, int samplePitch );

		void	_sendCreateEdgemap(GfxStreamEncoder * pEncoder);
		void 	_sendSamples( GfxStreamEncoder* pEncoder, int edgeBegin, int edgeEnd,
								int sampleBegin, int sampleEnd );

		GfxStream::SpxFormat _findBestPackFormat( int spxMask );

		
		GfxStreamEncoder_p	m_pEncoder;
		uint16_t			m_inStreamId;		// Id of this edgemap in the stream.

		bool				m_bEmpty = true;
		
		char *				m_pBuffer;
		spx *				m_pSamples;					// Stored vertically, e.g. samples for first column for all edges before samples for second column etc
		
   };



} // namespace wg
#endif // WG_GFXSTREAMEDGEMAP_DOT_H
