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
#ifndef WG_SOFTSURFACE_DOT_H
#define WG_SOFTSURFACE_DOT_H
#pragma once

#include <wg_surface.h>

#include <vector>

namespace wg
{

	class SoftSurface;
	typedef	StrongPtr<SoftSurface>	SoftSurface_p;
	typedef	WeakPtr<SoftSurface>		SoftSurface_wp;

	//____ Class SoftSurface _____________________________________________________________________

	class SoftSurface : public Surface
	{
		friend class SoftGfxDevice;
		friend class SoftSurfaceFactory;

	 public:

		 //.____ Creation __________________________________________

		static SoftSurface_p	create( SizeI size, PixelFormat format = PixelFormat::BGRA_8, int flags = SurfaceFlag::Static, const Color8 * pClut = nullptr );
		static SoftSurface_p	create( SizeI size, PixelFormat format, Blob * pBlob, int pitch, int flags = SurfaceFlag::Static, const Color8 * pClut = nullptr );
		static SoftSurface_p	create( SizeI size, PixelFormat format, uint8_t * pPixels, int pitch, const PixelDescription * pPixelDescription = nullptr, int flags = SurfaceFlag::Static, const Color8 * pClut = nullptr);
		static SoftSurface_p	create( Surface * pOther, int flags = SurfaceFlag::Static );

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Geometry _________________________________________________

		static SizeI	maxSize();

		//.____ Appearance ____________________________________________________

		bool		isOpaque() const override;
		bool		setTiling(bool bTiling) override;

		//.____ Content _______________________________________________________

		uint8_t		alpha( CoordI coord ) override;

		//.____ Control _______________________________________________________

		const PixelBuffer	allocPixelBuffer(const RectI& rect) override;
		bool				pushPixels(const PixelBuffer& buffer, const RectI& bufferRect) override;
		void				pullPixels(const PixelBuffer& buffer, const RectI& bufferRect) override;
		void				freePixelBuffer(const PixelBuffer& buffer) override;

		//.____  Internal ____________________________________________________

		void putPixels(const std::vector<int> &x, const std::vector<int> &y, const std::vector<uint32_t> &col, int length, bool replace);


	protected:
		SoftSurface( SizeI size, PixelFormat format, int flags, const Color8 * pClut );
		SoftSurface( SizeI size, PixelFormat format, Blob * pBlob, int pitch, int flags, const Color8 * pClut );
		SoftSurface( SizeI size, PixelFormat format, uint8_t * pPixels, int pitch, const PixelDescription * pPixelDescription, int flags, const Color8 * pClut );
		SoftSurface( Surface * pOther, int flags );

		virtual ~SoftSurface();

		void		_makeClut4096();

		int			m_srcPosMaskX = 0;
		int			m_srcPosMaskY = 0;

		int			m_pitch;

		Blob_p		m_pBlob;
		uint8_t*	m_pData;

		int16_t*	m_pClut4096 = nullptr;
	};


	//========================================================================================

} // namespace wg
#endif // WG_SOFTSURFACE_DOT_H
