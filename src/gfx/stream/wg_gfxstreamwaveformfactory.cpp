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

#include <wg_gfxstreamwaveformfactory.h>
#include <wg_gfxstreamwaveform.h>

namespace wg
{

	const TypeInfo GfxStreamWaveformFactory::TYPEINFO = { "GfxStreamWaveformFactory", &WaveformFactory::TYPEINFO };

	//____ typeInfo() _________________________________________________________

	const TypeInfo& GfxStreamWaveformFactory::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ createWaveform() __________________________________________________________


	Waveform_p GfxStreamWaveformFactory::createWaveform( const Waveform::Blueprint& blueprint )
	{
		return GfxStreamWaveform::create(m_pEncoder, blueprint);
	}	

	Waveform_p GfxStreamWaveformFactory::createWaveform( const Waveform::Blueprint& blueprint, WaveOrigo origo, const float * pSamples, int edges, int edgePitch, int samplePitch )
	{
		return GfxStreamWaveform::create(m_pEncoder, blueprint, origo, pSamples, edges, edgePitch, samplePitch);
	}

	Waveform_p GfxStreamWaveformFactory::createWaveform( const Waveform::Blueprint& blueprint, WaveOrigo origo, const spx * pSamples, int edges, int edgePitch, int samplePitch )
	{
		return GfxStreamWaveform::create(m_pEncoder, blueprint, origo, pSamples, edges, edgePitch, samplePitch);
	}


} // namespace wg
