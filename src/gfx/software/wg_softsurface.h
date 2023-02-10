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

		static SoftSurface_p	create(const Blueprint& blueprint);
		static SoftSurface_p	create(const Blueprint& blueprint, Blob* pBlob, int pitch = 0 );
		static SoftSurface_p	create(const Blueprint& blueprint, uint8_t* pPixels, int pitch = 0, const PixelDescription* pPixelDescription = nullptr);
		static SoftSurface_p	create(const Blueprint& blueprint, Surface* pOther);

		static SoftSurface_p	createInPlace(const Blueprint& blueprint, uint8_t* pPixels, int pitch = 0);

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Geometry _________________________________________________

		static SizeI	maxSize();

		//.____ Content _______________________________________________________

		int				alpha( CoordSPX coord ) override;

		//.____ Control _______________________________________________________

		const PixelBuffer	allocPixelBuffer(const RectI& rect) override;
		bool				pushPixels(const PixelBuffer& buffer, const RectI& bufferRect) override;
		void				pullPixels(const PixelBuffer& buffer, const RectI& bufferRect) override;
		void				freePixelBuffer(const PixelBuffer& buffer) override;

		//.____ Internal ______________________________________________________
		
		inline int			pitch() const { return m_pitch; }
		inline uint8_t *	pixels() const { return m_pData; }
		inline int			tileMaskX() const { return m_srcPosMaskX; }
		inline int			tileMaskY() const { return m_srcPosMaskY; }
		inline const HiColor * palette4096() const { return m_pPalette4096; }
		
		//.____ Deprecated ____________________________________________________

		static SoftSurface_p	create(SizeI size, PixelFormat format = PixelFormat::BGRA_8, int flags = SurfaceFlag::Static, const Color8* pPalette = nullptr);
		static SoftSurface_p	create(SizeI size, PixelFormat format, Blob* pBlob, int pitch, int flags = SurfaceFlag::Static, const Color8* pPalette = nullptr);
		static SoftSurface_p	create(SizeI size, PixelFormat format, uint8_t* pPixels, int pitch, const PixelDescription* pPixelDescription = nullptr, int flags = SurfaceFlag::Static, const Color8* pPalette = nullptr);
		static SoftSurface_p	create(Surface* pOther, int flags = SurfaceFlag::Static);

		void putPixels(const std::vector<int> &x, const std::vector<int> &y, const std::vector<uint32_t> &col, int length, bool replace);


	protected:
		SoftSurface(const Blueprint& blueprint);
		SoftSurface(const Blueprint& blueprint, Blob* pBlob, int pitch);
		SoftSurface(const Blueprint& blueprint, uint8_t* pPixels, int pitch);		// Create in place!
		SoftSurface(const Blueprint& blueprint, uint8_t* pPixels, int pitch, const PixelDescription* pPixelDescription);
		SoftSurface(const Blueprint& blueprint, Surface* pOther);

		~SoftSurface();

		void		_makePalette4096();
		void		_initTiling();

		int			m_srcPosMaskX = 0;
		int			m_srcPosMaskY = 0;

		int			m_pitch;

		Blob_p		m_pBlob;
		uint8_t*	m_pData;

		HiColor*	m_pPalette4096 = nullptr;
	};


	//========================================================================================

} // namespace wg
#endif // WG_SOFTSURFACE_DOT_H
