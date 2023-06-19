/*=========================================================================

						 >>> WonderGUI <<<

  This file is part of Tord Jansson's WonderGUI Graphics Toolkit
  and copyright (c) Tord Jansson, Sweden [tord.jansson@gmail.com].

							-----------

  The WonderGUI Graphics Toolkit is free Glware; you can redistribute
  this file and/or modify it under the terms of the GNU General Public
  License as published by the Free Glware Foundation; either
  version 2 of the License, or (at your option) any later version.

							-----------

  The WonderGUI Graphics Toolkit is also available for use in commercial
  closed-source projects under a separate license. Interested parties
  should contact Tord Jansson [tord.jansson@gmail.com] for details.

=========================================================================*/

#ifndef WG_METALWAVEFORM_DOT_H
#define WG_METALWAVEFORM_DOT_H
#pragma once

#include <wg_waveform.h>

namespace wg
{

	class MetalWaveform;
	typedef StrongPtr<MetalWaveform> MetalWaveform_p;
	typedef WeakPtr<MetalWaveform> MetalWaveform_wp;

	class MetalWaveform : public Waveform
	{
		friend class MetalGfxDevice;
	public:

		//.____ Creation __________________________________________
		
		static MetalWaveform_p	create( const Blueprint& blueprint );
		static MetalWaveform_p	create( const Waveform::Blueprint& blueprint, WaveOrigo origo, const float * pSamples, int edges, int edgePitch = 0, int samplePitch = 0);
		static MetalWaveform_p	create( const Waveform::Blueprint& blueprint, WaveOrigo origo, const spx * pSamples, int edges, int edgePitch = 0, int samplePitch = 0);


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

		MetalWaveform(const Blueprint& bp);
		~MetalWaveform();

		void 	_importSamples( WaveOrigo origo, const spx * pSource, int edgeBegin, int edgeEnd,
									  int sampleBegin, int sampleEnd, int edgePitch, int samplePitch );

		void 	_importSamples( WaveOrigo origo, const float * pSource, int edgeBegin, int edgeEnd,
									  int sampleBegin, int sampleEnd, int edgePitch, int samplePitch );

		
		char *	m_pBuffer;
		spx *	m_pSamples;					// Stored vertically, e.g. samples for first column for all edges before samples for second column etc

		HiColor *	m_pRenderColors;		// Palette to be used in render call, converted as needed.
		TintMode	m_tintMode;
		
   };



} // namespace wg
#endif // WG_METALWAVEFORM_DOT_H
