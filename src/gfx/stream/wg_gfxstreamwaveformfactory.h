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

#ifndef WG_GFXSTREAMWAVEFORMFACTORY_DOT_H
#define	WG_GFXSTREAMWAVEFORMFACTORY_DOT_H
#pragma once


#include <wg_waveformfactory.h>
#include <wg_gfxstreamencoder.h>

namespace wg
{


	class GfxStreamWaveformFactory;
	typedef	StrongPtr<GfxStreamWaveformFactory>	GfxStreamWaveformFactory_p;
	typedef	WeakPtr<GfxStreamWaveformFactory>	GfxStreamWaveformFactory_wp;


	//____ GfxStreamWaveformFactory _______________________________________________________

	class GfxStreamWaveformFactory : public WaveformFactory
	{
	public:

		//.____ Creation __________________________________________

		static GfxStreamWaveformFactory_p	create( GfxStreamEncoder * pEncoder ) { return GfxStreamWaveformFactory_p(new GfxStreamWaveformFactory()); }


		//.____ Identification _________________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Misc _______________________________________________________

		Waveform_p createWaveform( const Waveform::Blueprint& blueprint) override;	
		Waveform_p createWaveform( const Waveform::Blueprint& blueprint, WaveOrigo origo, float * pSamples, int edges, int edgePitch = 0, int samplePitch = 0) override;
		Waveform_p createWaveform( const Waveform::Blueprint& blueprint, WaveOrigo origo, spx * pSamples, int edges, int edgePitch = 0, int samplePitch = 0) override;

	protected:
		virtual ~GfxStreamWaveformFactory() {}
		
		GfxStreamEncoder_p	m_pEncoder;
	};


	//==============================================================================

} // namespace wg
#endif // WG_GFXSTREAMWAVEFORMFACTORY_DOT_H






