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

#ifndef WG_SURFACE_DOT_H
#define	WG_SURFACE_DOT_H
#pragma once
//==============================================================================

#include <wg_types.h>
#include <wg_geo.h>
#include <wg_color.h>
#include <wg_pointers.h>
#include <wg_blob.h>

namespace wg
{


	class Surface;
	typedef	StrongPtr<Surface>	Surface_p;
	typedef	WeakPtr<Surface>	Surface_wp;

	class BasicDataset;
	typedef	StrongPtr<BasicDataset>	BasicDataset_p;
	typedef	WeakPtr<BasicDataset>	BasicDataset_wp;


	//____ PixelBuffer ________________________________________________________

	struct PixelBuffer
	{
		PixelFormat	format;
		uint8_t*	pPixels;
		Color8*		pClut;
		RectI		rect;
		int			pitch;
	};

	//____ NinePatch ___________________________________________________________

	struct NinePatch
	{
		RectI		block;
		BorderI		frame;

		int			rigidPartXOfs = 0;
		int			rigidPartXLength = 0;
		YSections	rigidPartXSections = YSections::None;

		int			rigidPartYOfs = 0;
		int			rigidPartYLength = 0;
		XSections	rigidPartYSections = XSections::None;
	};



	//____ Surface ______________________________________________________________
	/**
	 * @brief Simple abstract representation of a bitmap image.
	 *
	 * A Surface is a simple bitmap image that can hold graphical elements for
	 * rendering of the GUI such as skin blocks, fonts and icons. Surfaces normally
	 * acts as the source of graphical copy operations but can also be used as destinations
	 * when using offscreen surfaces as rendering caches.
	 *
	 *
	 *
	 **/


	class Surface : public Object
	{
		friend class GfxDevice;

	public:
		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		inline void         setIdentity(int id);
		inline int          identity() const;

		//.____ Geometry _________________________________________________

		inline SizeI		size() const;						///< @brief Get the size in pixels of the surface.
		virtual	int			width() const;						///< @brief Get the width of the surface.
		virtual	int			height() const;						///< @brief Get the height of the surface.

		bool                setScale( float scale );
		inline float        scale() const;

		inline SizeI        pointSize() const;
		inline int			qpixPerPoint() const;

		//.____ Appearance ____________________________________________________

		virtual void		setScaleMode( ScaleMode mode );
		inline ScaleMode	scaleMode() const;

		virtual void		setTiling(bool bTiling);
		inline bool			isTiling() const { return m_bTiling; }
		
		inline bool			isMipmapped() const;

		//.____ Content _______________________________________________________

		virtual uint8_t		alpha( CoordI coord ) = 0;	///< @brief Get Alpha value of pixel at specified coordinate.
		inline uint8_t		alpha( int x, int y );		///< @brief Get Alpha value of pixel at specified coordinate.

		virtual	uint32_t	colorToPixel( const HiColor& col ) const;		///< @brief Convert specified color to a pixel in surface's native format.
		virtual	HiColor		pixelToColor( uint32_t pixel ) const;		///< @brief Get the color and alpha values of a pixel.

		inline const Color8* clut() const;

		inline const PixelDescription*	pixelDescription() const; ///< @brief Get the pixel description for the surface.
		inline PixelFormat	pixelFormat() const;
		inline int			pixelBytes() const;

		virtual bool		isOpaque() const = 0;				///< @brief Check if surface is entirely opaque.

		//.____ Control _______________________________________________________

		inline const PixelBuffer allocPixelBuffer() { return allocPixelBuffer(m_size); }
		virtual const PixelBuffer allocPixelBuffer(const RectI& rect) = 0;

        inline bool			pushPixels(const PixelBuffer& buffer) { return pushPixels(buffer, buffer.rect.size()); }
		virtual bool		pushPixels(const PixelBuffer& buffer, const RectI& bufferRect) = 0;

        inline void			pullPixels(const PixelBuffer& buffer) { pullPixels(buffer, buffer.rect.size()); }
		virtual void		pullPixels(const PixelBuffer& buffer, const RectI& bufferRect) = 0;

		virtual void		freePixelBuffer(const PixelBuffer& buffer) = 0;


		//.____  Rendering ____________________________________________________

		virtual bool		fill( HiColor col );								///< @brief Fill surface with specified color.
		virtual bool		fill( HiColor col, const RectI& region );			///< @brief Fill section of surface with specified color
		virtual bool		copyFrom( Surface * pSrcSurf, const RectI& srcRect, CoordI dst );	///< @brief Copy block of graphics from other surface
		virtual bool		copyFrom( Surface * pSrcSurf, CoordI dst );		///< @brief Copy other surface as a block

		//.____ Misc _________________________________________________________

		void				setBaggage(Object * pBaggage);
		Object_p			baggage() const;

	protected:
		Surface( int flags );
		virtual ~Surface();

		static const uint8_t *	s_pixelConvTabs[9];

		bool 				_copyFrom( const PixelDescription * pSrcFormat, uint8_t * pSrcPixels, int srcPitch, const RectI& srcRect, const RectI& dstRect, const Color8 * pCLUT = nullptr );

		int                 m_id = 0;

        int             	m_flags;                              // The flags provided to the constructor.
		
		PixelDescription	m_pixelDescription;
		SizeI				m_size;				// Width and height in pixels.

		ScaleMode			m_scaleMode = ScaleMode::Nearest;
		bool				m_bMipmapped = false;
		bool				m_bTiling = false;

		Color8 *			m_pClut = nullptr;					// Pointer at color lookup table. Always 256 entries long.

		Object_p			m_pBaggage;

		// This is currently here just as metadata for WG2 compatibility, but needs to be factored in correctly in the future
		int                 m_qpixPerPoint = 4;
	};

	//____ setIdentity() ____________________________________________________________

	void Surface::setIdentity(int id)
	{
		m_id = id;
	}

	//____ identity() _______________________________________________________________

	int Surface::identity() const
	{
		return m_id;
	}

	//____ size() _____________________________________________________________
	/**
	 * Get the width and height of the surface in a SizeI structure.
	 *
	 * @return Size of the suface measured in pixels.
	 */
	SizeI Surface::size() const
	{
		return m_size;
	}

	//____ scale() ____________________________________________________________

	float Surface::scale() const
	{
		return m_qpixPerPoint / 4.f;
	}

	//____ pointSize() ________________________________________________________

	SizeI Surface::pointSize() const
	{
		return (m_size * 4) / m_qpixPerPoint;
	}

	//____ qpixPerPoint() ____________________________________________

	int Surface::qpixPerPoint() const
	{
		return m_qpixPerPoint;
	}

	//____ scaleMode() ________________________________________________________

	ScaleMode Surface::scaleMode() const
	{
		return m_scaleMode;
	}

	//____ isMipmapped() ______________________________________________________

	bool Surface::isMipmapped() const
	{
		return m_bMipmapped;
	}

	//____ clut() _____________________________________________________________

	const Color8* Surface::clut() const
	{
		return m_pClut;
	}

	//____ pixelDescription() _________________________________________________

	const PixelDescription*	Surface::pixelDescription() const
	{
		return &m_pixelDescription;
	}

	//____ pixelFormat() ______________________________________________________

	PixelFormat Surface::pixelFormat() const
	{
		return m_pixelDescription.format;
	}

	//____ pixelBytes() _______________________________________________________

	int Surface::pixelBytes() const
	{
		return m_pixelDescription.bits / 8;
	}

	//____ alpha() ____________________________________________________________
	/**
	 * Get the alpha value from the specified coordinate of the surface.
	 *
	 * @param x X-coordinate of pixel.
	 * @param y Y-coordinate of pixel.
	 *
	 * Gets the alpha value of the pixel at the specified coordinate in the range of 0-255.
	 * The alpha value is by default used for opacity level,
	 * where 0 is a fully transparent pixel and 255 is opaque.
	 * The coordinate specified must be within the surface boundaries. A coordinate outside
	 * the surface will result in undefined behavior.
	 *
	 * @return Alpha value of pixel at coordinate.
	 */
	uint8_t Surface::alpha( int x, int y )
	{
		return alpha( CoordI(x,y) );
	}

	//==============================================================================

} // namespace wg
#endif // WG_SURFACE_DOT_H
