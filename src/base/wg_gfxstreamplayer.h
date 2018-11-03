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

#ifndef	WG_GFXSTREAMPLAYER_DOT_H
#define	WG_GFXSTREAMPLAYER_DOT_H
#pragma once

#include <wg_object.h>
#include <wg_gfxinstream.h>
#include <wg_gfxdevice.h>

#include <vector>

namespace wg
{

	class GfxStreamPlayer;
	typedef	StrongPtr<GfxStreamPlayer>	GfxStreamPlayer_p;
	typedef	WeakPtr<GfxStreamPlayer>	GfxStreamPlayer_wp;

	class GfxStreamPlayer : public Object
	{
	public:

		//.____ Creation __________________________________________

		static GfxStreamPlayer_p	create(GfxInStream& In, GfxDevice * pDevice, SurfaceFactory * pFactory);

		//.____ Identification __________________________________________

		bool				isInstanceOf(const char * pClassName) const;
		const char *		className(void) const;
		static const char	CLASSNAME[];
		static GfxStreamPlayer_p	cast(Object * pObject);

		//.____ Control _______________________________________________________

		bool		isEmpty() const;
		GfxStream::Header	peekChunk() const;

		void		playAll();
		bool		playChunk();
		bool		playFrame();

	protected:
		GfxStreamPlayer(GfxInStream& in, GfxDevice * pDevice, SurfaceFactory * pFactory);
		~GfxStreamPlayer();

		int _unpackPatches();

		Rect				m_patches[GfxStream::c_maxPatches];

		GfxInStream_p		m_pStream;
		GfxDevice_p			m_pDevice;
		SurfaceFactory_p	m_pSurfaceFactory;

		std::vector<Surface_p>	m_vSurfaces;

		Surface_p			m_pUpdatingSurface;
		uint8_t *			m_pWritePixels;

	};

}

#endif // WG_GFXSTREAMPLAYER_DOT_H