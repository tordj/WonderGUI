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
#include <wg_pluginedgemap.h>


namespace wg
{
	const TypeInfo PluginEdgemap::TYPEINFO = { "PluginEdgemap", &Edgemap::TYPEINFO };


	//____ create ______________________________________________________________

	PluginEdgemap_p PluginEdgemap::create( wg_obj object)
	{
		return PluginEdgemap_p(new PluginEdgemap(object));
	}

	//____ constructor _____________________________________________________________

	PluginEdgemap::PluginEdgemap(wg_obj object) : Edgemap( Blueprint() )
	{
		PluginCalls::object->retain(object);

		m_cEdgemap = object;

		// We have initialized with an empty Blueprint. Now lets set correct values.

		wg_sizeI pixSize	= PluginCalls::edgemap->edgemapPixelSize(object);
		m_size				= *(SizeI*)&pixSize;
		m_nbSegments		= PluginCalls::edgemap->edgemapSegments(object);
		m_nbRenderSegments  = PluginCalls::edgemap->getRenderSegments(object);
	}

	//____ Destructor ______________________________________________________________

	PluginEdgemap::~PluginEdgemap()
	{
		PluginCalls::object->release(m_cEdgemap);
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& PluginEdgemap::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ color() ____________________________________________________________

	HiColor PluginEdgemap::color(int segment) const
	{
		auto color = PluginCalls::edgemap->edgemapColor(m_cEdgemap, segment);
		return *(HiColor*)&color;
	}

	//____ gradient() _________________________________________________________

	Gradient PluginEdgemap::gradient(int segment) const
	{
		auto gradient = PluginCalls::edgemap->edgemapGradient(m_cEdgemap, segment);
		return *(Gradient*)&gradient;
	}

	//____ importSamples() ____________________________________________________

	bool PluginEdgemap::importSamples(WaveOrigo origo, const spx* pSource, int edgeBegin, int edgeEnd,
		int sampleBegin, int sampleEnd, int edgePitch, int samplePitch)
	{
		return (bool) PluginCalls::edgemap->importSpxSamples(m_cEdgemap, (wg_waveOrigo)origo, pSource, edgeBegin, edgeEnd,
			sampleBegin, sampleEnd, edgePitch, samplePitch);
	}

	bool PluginEdgemap::importSamples(WaveOrigo origo, const float* pSource, int edgeBegin, int edgeEnd,
		int sampleBegin, int sampleEnd, int edgePitch, int samplePitch)
	{
		return (bool)PluginCalls::edgemap->importFloatSamples(m_cEdgemap, (wg_waveOrigo)origo, pSource, edgeBegin, edgeEnd,
			sampleBegin, sampleEnd, edgePitch, samplePitch);
	}

	//____ exportSamples() ____________________________________________________

	bool PluginEdgemap::exportSamples(WaveOrigo origo, spx* pDestination, int edgeBegin, int edgeEnd,
		int sampleBegin, int sampleEnd, int edgePitch, int samplePitch)
	{
		return (bool)PluginCalls::edgemap->exportSpxSamples(m_cEdgemap, (wg_waveOrigo)origo, pDestination, edgeBegin, edgeEnd,
			sampleBegin, sampleEnd, edgePitch, samplePitch);
	}

	bool PluginEdgemap::exportSamples(WaveOrigo origo, float* pDestination, int edgeBegin, int edgeEnd,
		int sampleBegin, int sampleEnd, int edgePitch, int samplePitch)
	{
		return (bool)PluginCalls::edgemap->exportFloatSamples(m_cEdgemap, (wg_waveOrigo)origo, pDestination, edgeBegin, edgeEnd,
			sampleBegin, sampleEnd, edgePitch, samplePitch);
	}


} // namespace wg
