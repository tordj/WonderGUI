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

#ifndef WG_GFXSTREAMWAVEFORM_DOT_H
#define WG_GFXSTREAMWAVEFORM_DOT_H
#pragma once

#include <wg_waveform.h>
#include <wg_gfxstreamencoder.h>

namespace wg
{

	class GfxStreamWaveform;
	typedef StrongPtr<GfxStreamWaveform> GfxStreamWaveform_p;
	typedef WeakPtr<GfxStreamWaveform> GfxStreamWaveform_wp;

	class GfxStreamWaveform : public Waveform
	{
		friend class SoftGfxDevice;
	public:

		//.____ Creation __________________________________________
		
		static GfxStreamWaveform_p	create( GfxStreamEncoder * pEncoder, const Blueprint& blueprint );
		static GfxStreamWaveform_p	create( GfxStreamEncoder * pEncoder, const Waveform::Blueprint& blueprint, WaveOrigo origo, float * pSamples, int edges, int edgePitch = 0, int samplePitch = 0);
		static GfxStreamWaveform_p	create( GfxStreamEncoder * pEncoder, const Waveform::Blueprint& blueprint, WaveOrigo origo, spx * pSamples, int edges, int edgePitch = 0, int samplePitch = 0);


    	//.____ Identification __________________________________________

		const TypeInfo&     typeInfo(void) const override;
		const static TypeInfo   TYPEINFO;


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

		GfxStreamWaveform( GfxStreamEncoder * pEncoder, const Blueprint& bp );
		~GfxStreamWaveform();

		void 	_importSamples( WaveOrigo origo, const spx * pSource, int edgeBegin, int edgeEnd,
									  int sampleBegin, int sampleEnd, int edgePitch, int samplePitch );

		void 	_importSamples( WaveOrigo origo, const float * pSource, int edgeBegin, int edgeEnd,
									  int sampleBegin, int sampleEnd, int edgePitch, int samplePitch );

		void	_sendCreateWaveform(GfxStreamEncoder * pEncoder);
		void 	_sendSamples( GfxStreamEncoder* pEncoder, int edgeBegin, int edgeEnd,
							 int sampleBegin, int sampleEnd );

		
		GfxStreamEncoder_p	m_pEncoder;
		uint16_t			m_inStreamId;		// Id of this waveform in the stream.

		char *				m_pBuffer;
		spx *				m_pSamples;					// Stored vertically, e.g. samples for first column for all edges before samples for second column etc
		
   };



} // namespace wg
#endif // WG_GFXSTREAMWAVEFORM_DOT_H
