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

#include <wg_streamedgemapfactory.h>
#include <wg_streamedgemap.h>

namespace wg
{

	const TypeInfo StreamEdgemapFactory::TYPEINFO = { "StreamEdgemapFactory", &EdgemapFactory::TYPEINFO };

	//____ typeInfo() _________________________________________________________

	const TypeInfo& StreamEdgemapFactory::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ createEdgemap() __________________________________________________________


	Edgemap_p StreamEdgemapFactory::createEdgemap( const Edgemap::Blueprint& blueprint )
	{
		return StreamEdgemap::create(m_pEncoder, blueprint);
	}	

	Edgemap_p StreamEdgemapFactory::createEdgemap( const Edgemap::Blueprint& blueprint, SampleOrigo origo, const float * pSamples, int edges, int edgePitch, int samplePitch )
	{
		return StreamEdgemap::create(m_pEncoder, blueprint, origo, pSamples, edges, edgePitch, samplePitch);
	}

	Edgemap_p StreamEdgemapFactory::createEdgemap( const Edgemap::Blueprint& blueprint, SampleOrigo origo, const spx * pSamples, int edges, int edgePitch, int samplePitch )
	{
		return StreamEdgemap::create(m_pEncoder, blueprint, origo, pSamples, edges, edgePitch, samplePitch);
	}


} // namespace wg
