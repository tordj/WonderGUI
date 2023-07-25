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
#include <wg_pluginedgemap.h>
#include <wg_pluginedgemapfactory.h>


namespace wg
{

	const TypeInfo PluginEdgemapFactory::TYPEINFO = { "PluginEdgemapFactory", &EdgemapFactory::TYPEINFO };

	//____ constructor ________________________________________________________

	PluginEdgemapFactory::PluginEdgemapFactory( wg_obj cFactory) : m_cFactory(cFactory)
	{
		PluginCalls::object->retain(cFactory);
	}

	//____ destructor ________________________________________________________

	PluginEdgemapFactory::~PluginEdgemapFactory()
	{
		PluginCalls::object->release(m_cFactory);
	}


	//____ typeInfo() _________________________________________________________

	const TypeInfo& PluginEdgemapFactory::typeInfo(void) const
	{
		return TYPEINFO;
	}


	//____ createEdgemap() ________________________________________________________

	Edgemap_p PluginEdgemapFactory::createEdgemap(const Edgemap::Blueprint& blueprint)
	{
		auto obj = PluginCalls::edgemapFactory->createEdgemap(m_cFactory, (wg_edgemapBP*) &blueprint);
		auto pEdgemap = PluginEdgemap::create(obj);
		PluginCalls::object->release(obj);
		return pEdgemap;
	}

	Edgemap_p PluginEdgemapFactory::createEdgemap(const Edgemap::Blueprint& blueprint, SampleOrigo origo, const float* pSamples, int edges, int edgePitch, int samplePitch)
	{
		auto obj = PluginCalls::edgemapFactory->createEdgemapFromFloats(m_cFactory, (wg_edgemapBP*)&blueprint, (wg_sampleOrigo) origo, pSamples, edges, edgePitch, samplePitch);
		auto pEdgemap = PluginEdgemap::create(obj);
		PluginCalls::object->release(obj);
		return pEdgemap;
	}

	Edgemap_p PluginEdgemapFactory::createEdgemap(const Edgemap::Blueprint& blueprint, SampleOrigo origo, const spx* pSamples, int edges, int edgePitch, int samplePitch)
	{
		auto obj = PluginCalls::edgemapFactory->createEdgemapFromSpx(m_cFactory, (wg_edgemapBP*)&blueprint, (wg_sampleOrigo)origo, pSamples, edges, edgePitch, samplePitch);
		auto pEdgemap = PluginEdgemap::create(obj);
		PluginCalls::object->release(obj);
		return pEdgemap;
	}


} // namespace wg
