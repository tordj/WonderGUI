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

#ifndef WG_STREAMEDGEMAP_DOT_H
#define WG_STREAMEDGEMAP_DOT_H
#pragma once

#include <wg_edgemap.h>
#include <wg_streamencoder.h>

namespace wg
{

	class StreamEdgemap;
	typedef StrongPtr<StreamEdgemap> StreamEdgemap_p;
	typedef WeakPtr<StreamEdgemap> StreamEdgemap_wp;

	class StreamEdgemap : public Edgemap
	{
		friend class StreamDevice;
	public:

		//.____ Creation __________________________________________
		
		static StreamEdgemap_p	create( StreamEncoder * pEncoder, const Blueprint& blueprint );
		static StreamEdgemap_p	create( StreamEncoder * pEncoder, const Edgemap::Blueprint& blueprint, SampleOrigo origo, const float * pSamples, int edges, int edgePitch = 0, int samplePitch = 0);
		static StreamEdgemap_p	create( StreamEncoder * pEncoder, const Edgemap::Blueprint& blueprint, SampleOrigo origo, const spx * pSamples, int edges, int edgePitch = 0, int samplePitch = 0);


    	//.____ Identification __________________________________________

		const TypeInfo&     typeInfo(void) const override;
		const static TypeInfo   TYPEINFO;

		//.____ Appearance ____________________________________________________

		bool	setRenderSegments(int nSegments) override;

		bool	setColors( int begin, int end, const HiColor * pColors ) override;
		bool	setGradients( int begin, int end, const Gradient * pGradients ) override;
		
		//.____ Content _______________________________________________________

		bool 	importSamples( SampleOrigo origo, const spx * pSource, int edgeBegin, int edgeEnd,
									  int sampleBegin, int sampleEnd, int edgePitch = 0, int samplePitch = 0 ) override;

		bool 	importSamples( SampleOrigo origo, const float * pSource, int edgeBegin, int edgeEnd,
									  int sampleBegin, int sampleEnd, int edgePitch = 0, int samplePitch = 0 ) override;

		bool 	exportSamples( SampleOrigo origo, spx * pDestination, int edgeBegin, int edgeEnd,
									  int sampleBegin, int sampleEnd, int edgePitch = 0, int samplePitch = 0 ) override;

		bool 	exportSamples( SampleOrigo origo, float * pDestination, int edgeBegin, int edgeEnd,
									  int sampleBegin, int sampleEnd, int edgePitch = 0, int samplePitch = 0 ) override;

	protected:

		StreamEdgemap( StreamEncoder * pEncoder, const Blueprint& bp );
		~StreamEdgemap();

		void 	_importSamples( SampleOrigo origo, const spx * pSource, int edgeBegin, int edgeEnd,
								int sampleBegin, int sampleEnd, int edgePitch, int samplePitch );

		void 	_importSamples( SampleOrigo origo, const float * pSource, int edgeBegin, int edgeEnd,
								int sampleBegin, int sampleEnd, int edgePitch, int samplePitch );

		void	_sendCreateEdgemap(StreamEncoder * pEncoder);
		void 	_sendSamples( StreamEncoder* pEncoder, int edgeBegin, int edgeEnd,
								int sampleBegin, int sampleEnd );

		GfxStream::SpxFormat _findBestPackFormat( int spxMask );

		
		StreamEncoder_p	m_pEncoder;
		uint16_t			m_inStreamId;		// Id of this edgemap in the stream.

		bool				m_bEmpty = true;
		
		char *				m_pBuffer;
		spx *				m_pSamples;					// Stored vertically, e.g. samples for first column for all edges before samples for second column etc
		
   };



} // namespace wg
#endif // WG_STREAMEDGEMAP_DOT_H
