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

#ifndef	WG_CONTEXT_DOT_H
#define	WG_CONTEXT_DOT_H
#pragma once

#include <cstdint>

#include <wg_pointers.h>
#include <wg_surfacefactory.h>
#include <wg_gfxdevice.h>


namespace wg
{
	class Context;
	typedef	StrongPtr<Context>	Context_p;
	typedef	WeakPtr<Context>	Context_wp;

	class Context : public Object
	{
		friend class Base;

	public:

		//.____ Creation __________________________________________

		static Context_p	create() { return Context_p(new Context()); }

		//.____ Identification _________________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Content _______________________________________________________

		bool				setScale(float scale);
		inline float		scale() const { return m_scale; }

		void				setGfxDevice(GfxDevice * pDevice);
		inline GfxDevice_p	gfxDevice() const { return m_pGfxDevice; }

		void				setSurfaceFactory(SurfaceFactory * pDevice);
		inline SurfaceFactory_p surfaceFactory() const { return m_pSurfaceFactory; }


	private:
		Context();
		~Context();

		GfxDevice_p			m_pGfxDevice;		// GfxDevice that can be used by Widgets when needed.
		SurfaceFactory_p	m_pSurfaceFactory;	// SurfaceFactory that can be used by Widgets when needed.

		float				m_scale;			// Valid values are 1.0, 1.25, 1.5, 1.75, 2, 2.25, 2.5, 3.0, 3.5, 4.0, 4.5, 5.0, 6.0, 7.0, 8.0
	};

} //namespace
#endif //WG_CONTEXT_DOT_H
