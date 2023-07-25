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

#include <wg_gledgemapfactory.h>
#include <wg_gledgemap.h>

namespace wg
{



	const TypeInfo GlEdgemapFactory::TYPEINFO = { "GlEdgemapFactory", &EdgemapFactory::TYPEINFO };

	//____ typeInfo() _________________________________________________________

	const TypeInfo& GlEdgemapFactory::typeInfo(void) const
	{
		return TYPEINFO;
	}


	//____ createEdgemap() __________________________________________________________


	Edgemap_p GlEdgemapFactory::createEdgemap( const Edgemap::Blueprint& blueprint )
	{
		return GlEdgemap::create(blueprint);
	}	

	Edgemap_p GlEdgemapFactory::createEdgemap( const Edgemap::Blueprint& blueprint, SampleOrigo origo, const float * pSamples, int edges, int edgePitch, int samplePitch )
	{
		return GlEdgemap::create(blueprint, origo, pSamples, edges, edgePitch, samplePitch);
	}

	Edgemap_p GlEdgemapFactory::createEdgemap( const Edgemap::Blueprint& blueprint, SampleOrigo origo, const spx * pSamples, int edges, int edgePitch, int samplePitch )
	{
		return GlEdgemap::create(blueprint, origo, pSamples, edges, edgePitch, samplePitch);
	}


} // namespace wg
