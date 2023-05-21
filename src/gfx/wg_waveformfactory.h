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

#ifndef WG_WAVEFORMFACTORY_DOT_H
#define	WG_WAVEFORMFACTORY_DOT_H
#pragma once


#include <wg_waveform.h>

namespace wg
{


	class WaveformFactory;
	typedef	StrongPtr<WaveformFactory>	WaveformFactory_p;
	typedef	WeakPtr<WaveformFactory>	WaveformFactory_wp;


	//____ WaveformFactory _______________________________________________________
	/**
	 * @brief Factory class for creating waveforms.
	 *
	 * WaveformFactory is the base class for all waveform factories. The waveform factories
	 * are used by WonderGUI objects that needs to dynamically create waveforms as
	 * part of their operation.
	 *
	 **/
	class WaveformFactory : public Object
	{
	public:

		//.____ Identification _________________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Misc _______________________________________________________

		virtual Waveform_p createWaveform( const Waveform::Blueprint& blueprint) = 0;	
		virtual Waveform_p createWaveform( const Waveform::Blueprint& blueprint, WaveOrigo origo, const float * pSamples, int edges, int edgePitch = 0, int samplePitch = 0) = 0;
		virtual Waveform_p createWaveform( const Waveform::Blueprint& blueprint, WaveOrigo origo, const spx * pSamples, int edges, int edgePitch = 0, int samplePitch = 0) = 0;

	protected:
		virtual ~WaveformFactory() {}
	};


	//==============================================================================

} // namespace wg
#endif // WG_WAVEFORMFACTORY_DOT_H






