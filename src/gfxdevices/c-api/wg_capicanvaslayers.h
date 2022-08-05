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

#ifndef WG_CAPICANVASLAYERS_DOT_H
#define WG_CAPICANVASLAYERS_DOT_H
#pragma once

#include "wg_canvaslayers.h"


typedef void* wg_obj;

namespace wg
{
	class CAPISurface;
	class CAPIGfxDevice;
	typedef	StrongPtr<CAPIGfxDevice>	CAPIGfxDevice_p;
	typedef	WeakPtr<CAPIGfxDevice>	CAPIGfxDevice_wp;


	class CAPICanvasLayers;
	typedef	StrongPtr<CAPICanvasLayers>	CAPICanvasLayers_p;
	typedef	WeakPtr<CAPICanvasLayers>	CAPICanvasLayers_wp;

	//____ Class CAPICanvasLayers ________________________________________________________

	class	CAPICanvasLayers : public CanvasLayers
	{
		friend class CAPIGfxDevice;

	public:

		//.____ Blueprint _________________________________________

		// Uses same blueprint as base class.

		//.____ Creation ______________________________________________________

		static CAPICanvasLayers_p	create( CAPIGfxDevice * pDevice, const Blueprint& blueprint);

		//.____ Identification _________________________________________________

		const TypeInfo& typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Misc _______________________________________________________

		inline wg_obj		cObject() { return m_cCanvasLayer; }

	protected:
		CAPICanvasLayers( CAPIGfxDevice * pDevice, const Blueprint& bp);
		~CAPICanvasLayers();

		static void _callbackRouter( wg_obj cDevice, void* pMe, int funcNb );

		wg_obj			m_cCanvasLayer;
		CAPIGfxDevice_p	m_pDevice;
	};



} // namespace wg
#endif //WG_CANVASLAYERS_DOT_H
