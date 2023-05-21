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

#include <wg_softwaveformfactory.h>
#include <wg_softwaveform.h>

namespace wg
{



	const TypeInfo SoftWaveformFactory::TYPEINFO = { "SoftWaveformFactory", &WaveformFactory::TYPEINFO };

	//____ typeInfo() _________________________________________________________

	const TypeInfo& SoftWaveformFactory::typeInfo(void) const
	{
		return TYPEINFO;
	}


	//____ createWaveform() __________________________________________________________


	Waveform_p SoftWaveformFactory::createWaveform( const Waveform::Blueprint& blueprint )
	{
		return SoftWaveform::create(blueprint);
	}	

	Waveform_p SoftWaveformFactory::createWaveform( const Waveform::Blueprint& blueprint, WaveOrigo origo, const float * pSamples, int edges, int edgePitch, int samplePitch )
	{
		return SoftWaveform::create(blueprint, origo, pSamples, edges, edgePitch, samplePitch);
	}

	Waveform_p SoftWaveformFactory::createWaveform( const Waveform::Blueprint& blueprint, WaveOrigo origo, const spx * pSamples, int edges, int edgePitch, int samplePitch )
	{
		return SoftWaveform::create(blueprint, origo, pSamples, edges, edgePitch, samplePitch);
	}


} // namespace wg
