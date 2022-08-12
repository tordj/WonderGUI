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

#ifndef WG_CABICANVASLAYERS_DOT_H
#define WG_CABICANVASLAYERS_DOT_H
#pragma once

#include "wg_canvaslayers.h"


typedef void* wg_obj;

namespace wg
{
	class CABISurface;
	class CABIGfxDevice;
	typedef	StrongPtr<CABIGfxDevice>	CABIGfxDevice_p;
	typedef	WeakPtr<CABIGfxDevice>	CABIGfxDevice_wp;


	class CABICanvasLayers;
	typedef	StrongPtr<CABICanvasLayers>	CABICanvasLayers_p;
	typedef	WeakPtr<CABICanvasLayers>	CABICanvasLayers_wp;

	//____ Class CABICanvasLayers ________________________________________________________

	class	CABICanvasLayers : public CanvasLayers
	{
		friend class CABIGfxDevice;

	public:

		//.____ Blueprint _________________________________________

		// Uses same blueprint as base class.

		//.____ Creation ______________________________________________________

		static CABICanvasLayers_p	create( CABIGfxDevice * pDevice, const Blueprint& blueprint);

		//.____ Identification _________________________________________________

		const TypeInfo& typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Misc _______________________________________________________

		inline wg_obj		cObject() { return m_cCanvasLayer; }

	protected:
		CABICanvasLayers( CABIGfxDevice * pDevice, const Blueprint& bp);
		~CABICanvasLayers();

		static void _callbackRouter( wg_obj cDevice, void* pMe, int funcNb );

		wg_obj			m_cCanvasLayer;
		CABIGfxDevice_p	m_pDevice;
	};



} // namespace wg
#endif //WG_CANVASLAYERS_DOT_H
