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

#include <memory.h>

#include <wg_plugincalls.h>
#include <wg_pluginwaveform.h>
#include <wg_pluginwaveformfactory.h>


namespace wg
{

	const TypeInfo PluginWaveformFactory::TYPEINFO = { "PluginWaveformFactory", &WaveformFactory::TYPEINFO };

	//____ constructor ________________________________________________________

	PluginWaveformFactory::PluginWaveformFactory( wg_obj cFactory) : m_cFactory(cFactory)
	{
		PluginCalls::object->retain(cFactory);
	}

	//____ destructor ________________________________________________________

	PluginWaveformFactory::~PluginWaveformFactory()
	{
		PluginCalls::object->release(m_cFactory);
	}


	//____ typeInfo() _________________________________________________________

	const TypeInfo& PluginWaveformFactory::typeInfo(void) const
	{
		return TYPEINFO;
	}


	//____ createWaveform() ________________________________________________________

	Waveform_p PluginWaveformFactory::createWaveform(const Waveform::Blueprint& blueprint)
	{
		auto obj = PluginCalls::waveformFactory->createWaveform(m_cFactory, (wg_waveformBP*) &blueprint);
		auto pWaveform = PluginWaveform::create(obj);
		PluginCalls::object->release(obj);
		return pWaveform;
	}

	Waveform_p PluginWaveformFactory::createWaveform(const Waveform::Blueprint& blueprint, WaveOrigo origo, const float* pSamples, int edges, int edgePitch, int samplePitch)
	{
		auto obj = PluginCalls::waveformFactory->createWaveformFromFloats(m_cFactory, (wg_waveformBP*)&blueprint, (wg_waveOrigo) origo, pSamples, edges, edgePitch, samplePitch);
		auto pWaveform = PluginWaveform::create(obj);
		PluginCalls::object->release(obj);
		return pWaveform;
	}

	Waveform_p PluginWaveformFactory::createWaveform(const Waveform::Blueprint& blueprint, WaveOrigo origo, const spx* pSamples, int edges, int edgePitch, int samplePitch)
	{
		auto obj = PluginCalls::waveformFactory->createWaveformFromSpx(m_cFactory, (wg_waveformBP*)&blueprint, (wg_waveOrigo)origo, pSamples, edges, edgePitch, samplePitch);
		auto pWaveform = PluginWaveform::create(obj);
		PluginCalls::object->release(obj);
		return pWaveform;
	}


} // namespace wg
