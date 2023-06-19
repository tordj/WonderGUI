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

#ifndef WG_METALWAVEFORMFACTORY_DOT_H
#define	WG_METALWAVEFORMFACTORY_DOT_H
#pragma once


#include <wg_waveformfactory.h>

namespace wg
{


	class MetalWaveformFactory;
	typedef	StrongPtr<MetalWaveformFactory>	MetalWaveformFactory_p;
	typedef	WeakPtr<MetalWaveformFactory>	MetalWaveformFactory_wp;


	//____ MetalWaveformFactory _______________________________________________________

	class MetalWaveformFactory : public WaveformFactory
	{
	public:

		//.____ Creation __________________________________________

		static MetalWaveformFactory_p	create() { return MetalWaveformFactory_p(new MetalWaveformFactory()); }


		//.____ Identification _________________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Misc _______________________________________________________

		Waveform_p createWaveform( const Waveform::Blueprint& blueprint) override;	
		Waveform_p createWaveform( const Waveform::Blueprint& blueprint, WaveOrigo origo, const float * pSamples, int edges, int edgePitch = 0, int samplePitch = 0) override;
		Waveform_p createWaveform( const Waveform::Blueprint& blueprint, WaveOrigo origo, const spx * pSamples, int edges, int edgePitch = 0, int samplePitch = 0) override;

	protected:
		virtual ~MetalWaveformFactory() {}
	};


	//==============================================================================

} // namespace wg
#endif // WG_GLWAVEFORMFACTORY_DOT_H






