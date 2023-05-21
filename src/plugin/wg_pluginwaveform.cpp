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

#include <wg_plugincalls.h>
#include <wg_pluginwaveform.h>


namespace wg
{
	const TypeInfo PluginWaveform::TYPEINFO = { "PluginWaveform", &Waveform::TYPEINFO };


	//____ create ______________________________________________________________

	PluginWaveform_p PluginWaveform::create( wg_obj object)
	{
		return PluginWaveform_p(new PluginWaveform(object));
	}

	//____ constructor _____________________________________________________________

	PluginWaveform::PluginWaveform(wg_obj object) : Waveform( Blueprint() )
	{
		PluginCalls::object->retain(object);

		m_cWaveform = object;

		// We have initialized with an empty Blueprint. Now lets set correct values.

		wg_sizeI pixSize	= PluginCalls::waveform->waveformPixelSize(object);
		m_size				= *(SizeI*)&pixSize;
		m_nbSegments		= PluginCalls::waveform->waveformSegments(object);
		m_nbRenderSegments  = PluginCalls::waveform->getRenderSegments(object);
	}

	//____ Destructor ______________________________________________________________

	PluginWaveform::~PluginWaveform()
	{
		PluginCalls::object->release(m_cWaveform);
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& PluginWaveform::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ color() ____________________________________________________________

	HiColor PluginWaveform::color(int segment) const
	{
		auto color = PluginCalls::waveform->waveformColor(m_cWaveform, segment);
		return *(HiColor*)&color;
	}

	//____ gradient() _________________________________________________________

	Gradient PluginWaveform::gradient(int segment) const
	{
		auto gradient = PluginCalls::waveform->waveformGradient(m_cWaveform, segment);
		return *(Gradient*)&gradient;
	}

	//____ importSamples() ____________________________________________________

	bool PluginWaveform::importSamples(WaveOrigo origo, const spx* pSource, int edgeBegin, int edgeEnd,
		int sampleBegin, int sampleEnd, int edgePitch, int samplePitch)
	{
		return (bool) PluginCalls::waveform->importSpxSamples(m_cWaveform, (wg_waveOrigo)origo, pSource, edgeBegin, edgeEnd,
			sampleBegin, sampleEnd, edgePitch, samplePitch);
	}

	bool PluginWaveform::importSamples(WaveOrigo origo, const float* pSource, int edgeBegin, int edgeEnd,
		int sampleBegin, int sampleEnd, int edgePitch, int samplePitch)
	{
		return (bool)PluginCalls::waveform->importFloatSamples(m_cWaveform, (wg_waveOrigo)origo, pSource, edgeBegin, edgeEnd,
			sampleBegin, sampleEnd, edgePitch, samplePitch);
	}

	//____ exportSamples() ____________________________________________________

	bool PluginWaveform::exportSamples(WaveOrigo origo, spx* pDestination, int edgeBegin, int edgeEnd,
		int sampleBegin, int sampleEnd, int edgePitch, int samplePitch)
	{
		return (bool)PluginCalls::waveform->exportSpxSamples(m_cWaveform, (wg_waveOrigo)origo, pDestination, edgeBegin, edgeEnd,
			sampleBegin, sampleEnd, edgePitch, samplePitch);
	}

	bool PluginWaveform::exportSamples(WaveOrigo origo, float* pDestination, int edgeBegin, int edgeEnd,
		int sampleBegin, int sampleEnd, int edgePitch, int samplePitch)
	{
		return (bool)PluginCalls::waveform->exportFloatSamples(m_cWaveform, (wg_waveOrigo)origo, pDestination, edgeBegin, edgeEnd,
			sampleBegin, sampleEnd, edgePitch, samplePitch);
	}


} // namespace wg
