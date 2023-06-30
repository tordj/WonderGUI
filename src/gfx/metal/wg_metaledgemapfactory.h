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

#ifndef WG_METALEDGEMAPFACTORY_DOT_H
#define	WG_METALEDGEMAPFACTORY_DOT_H
#pragma once


#include <wg_edgemapfactory.h>

namespace wg
{


	class MetalEdgemapFactory;
	typedef	StrongPtr<MetalEdgemapFactory>	MetalEdgemapFactory_p;
	typedef	WeakPtr<MetalEdgemapFactory>	MetalEdgemapFactory_wp;


	//____ MetalEdgemapFactory _______________________________________________________

	class MetalEdgemapFactory : public EdgemapFactory
	{
	public:

		//.____ Creation __________________________________________

		static MetalEdgemapFactory_p	create() { return MetalEdgemapFactory_p(new MetalEdgemapFactory()); }


		//.____ Identification _________________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Misc _______________________________________________________

		Edgemap_p createEdgemap( const Edgemap::Blueprint& blueprint) override;	
		Edgemap_p createEdgemap( const Edgemap::Blueprint& blueprint, WaveOrigo origo, const float * pSamples, int edges, int edgePitch = 0, int samplePitch = 0) override;
		Edgemap_p createEdgemap( const Edgemap::Blueprint& blueprint, WaveOrigo origo, const spx * pSamples, int edges, int edgePitch = 0, int samplePitch = 0) override;

	protected:
		virtual ~MetalEdgemapFactory() {}
	};


	//==============================================================================

} // namespace wg
#endif // WG_GLEDGEMAPFACTORY_DOT_H






