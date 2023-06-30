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

#ifndef WG_GFXSTREAMEDGEMAPFACTORY_DOT_H
#define	WG_GFXSTREAMEDGEMAPFACTORY_DOT_H
#pragma once


#include <wg_edgemapfactory.h>
#include <wg_gfxstreamencoder.h>

namespace wg
{


	class GfxStreamEdgemapFactory;
	typedef	StrongPtr<GfxStreamEdgemapFactory>	GfxStreamEdgemapFactory_p;
	typedef	WeakPtr<GfxStreamEdgemapFactory>	GfxStreamEdgemapFactory_wp;


	//____ GfxStreamEdgemapFactory _______________________________________________________

	class GfxStreamEdgemapFactory : public EdgemapFactory
	{
	public:

		//.____ Creation __________________________________________

		static GfxStreamEdgemapFactory_p	create( GfxStreamEncoder * pEncoder ) { return GfxStreamEdgemapFactory_p(new GfxStreamEdgemapFactory(pEncoder)); }


		//.____ Identification _________________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Misc _______________________________________________________

		Edgemap_p createEdgemap( const Edgemap::Blueprint& blueprint) override;	
		Edgemap_p createEdgemap( const Edgemap::Blueprint& blueprint, WaveOrigo origo, const float * pSamples, int edges, int edgePitch = 0, int samplePitch = 0) override;
		Edgemap_p createEdgemap( const Edgemap::Blueprint& blueprint, WaveOrigo origo, const spx * pSamples, int edges, int edgePitch = 0, int samplePitch = 0) override;

	protected:
		GfxStreamEdgemapFactory(GfxStreamEncoder * pEncoder) : m_pEncoder(pEncoder) {}
		virtual ~GfxStreamEdgemapFactory() {}
		
		GfxStreamEncoder_p	m_pEncoder;
	};


	//==============================================================================

} // namespace wg
#endif // WG_GFXSTREAMEDGEMAPFACTORY_DOT_H






