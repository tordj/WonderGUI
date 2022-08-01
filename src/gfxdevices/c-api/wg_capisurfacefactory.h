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

#ifndef	WG_CAPISURFACEFACTORY_DOT_H
#define	WG_CAPISURFACEFACTORY_DOT_H
#pragma once

#include <wg_surfacefactory.h>

#include <vector>

typedef void* wg_obj;
class wg_surfaceBP;

namespace wg
{

	class CAPISurfaceFactory;
	typedef	StrongPtr<CAPISurfaceFactory>	CAPISurfaceFactory_p;
	typedef	WeakPtr<CAPISurfaceFactory>		CAPISurfaceFactory_wp;

	//____ CAPISurfaceFactory _____________________________________________________

	class CAPISurfaceFactory : public SurfaceFactory
	{
	public:
		//.____ Creation __________________________________________

		static CAPISurfaceFactory_p	create(wg_obj cFactory) { return CAPISurfaceFactory_p(new CAPISurfaceFactory(cFactory)); }

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Geometry _________________________________________________

		SizeI		maxSize() const override;

		//.____ Misc _______________________________________________________

		virtual Surface_p	createSurface(const Surface::Blueprint& blueprint) override;
		virtual Surface_p	createSurface(const Surface::Blueprint& blueprint, Blob* pBlob, int pitch) override;
		virtual Surface_p	createSurface(const Surface::Blueprint& blueprint, uint8_t* pPixels, int pitch, const PixelDescription* pPixelDescription) override;
		virtual Surface_p	createSurface(const Surface::Blueprint& blueprint, Surface* pOther) override;

	protected:
		CAPISurfaceFactory( wg_obj cFactory );
		~CAPISurfaceFactory();

		void _convertBlueprint(wg_surfaceBP* pDest, const Surface::Blueprint* pSource);


		wg_obj	m_cFactory;
	};
}


#endif //WG_CAPISURFACEFACTORY_DOT_H

