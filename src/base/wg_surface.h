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

		inline void         setId(int id);
		inline int          id() const;

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

		virtual uint32_t	pixel( CoordI coord ) const = 0;	///< @brief Get pixel at specified coordinate.
		inline uint32_t		pixel( int x, int y ) const;		///< @brief Get pixel at specified coordinate.

		virtual uint8_t		alpha( CoordI coord ) const = 0;	///< @brief Get Alpha value of pixel at specified coordinate.
		inline uint8_t		alpha( int x, int y ) const;		///< @brief Get Alpha value of pixel at specified coordinate.

		virtual	uint32_t	colorToPixel( const Color& col ) const;		///< @brief Convert specified color to a pixel in surface's native format.
		virtual	Color		pixelToColor( uint32_t pixel ) const;		///< @brief Get the color and alpha values of a pixel.

		inline const Color* clut() const;

		inline const PixelDescription*	pixelDescription() const; ///< @brief Get the pixel description for the surface.
		inline PixelFormat	pixelFormat() const;
		inline int			pixelBytes() const;

		virtual bool		isOpaque() const = 0;				///< @brief Check if surface is entirely opaque.

		//.____ Control _______________________________________________________

		virtual uint8_t *	lock( AccessMode mode ) = 0;					///< @brief Lock the surface for direct access to pixel data.

		virtual uint8_t *	lockRegion( AccessMode mode, const RectI& region ) = 0; ///< @brief Lock a limited region of the surface for direct access to pixel data.

		virtual void		unlock() = 0;									///< @brief Unlock a locked surface.

		inline 	bool		isLocked() const;								///< @brief Check if surface is locked.
		inline	AccessMode 	lockStatus() const;								///< @brief Check if surface is locked and in what way.

		inline  RectI		regionLocked() const;							///< @brief Get the locked region of the surface.
		inline  int			pitch() const;									///< @brief Get the pitch of the locked region.
		inline uint8_t *	pixels() const;									///< @brief Get a pointer to the raw pixels of the locked region.

		//.____  Rendering ____________________________________________________

		virtual bool		fill( Color col );								///< @brief Fill surface with specified color.
		virtual bool		fill( Color col, const RectI& region );			///< @brief Fill section of surface with specified color
		virtual bool		copyFrom( Surface * pSrcSurf, const RectI& srcRect, CoordI dst );	///< @brief Copy block of graphics from other surface
		virtual bool		copyFrom( Surface * pSrcSurf, CoordI dst );		///< @brief Copy other surface as a block

		//.____ Misc _________________________________________________________

		void				setBaggage(Object * pBaggage);
		Object_p			baggage() const;

	protected:
		Surface( int flags );
		virtual ~Surface();

		static const uint8_t *	s_pixelConvTabs[9];

		RectI				_lockAndAdjustRegion( AccessMode modeNeeded, const RectI& region );
		bool 				_copyFrom( const PixelDescription * pSrcFormat, uint8_t * pSrcPixels, int srcPitch, const RectI& srcRect, const RectI& dstRect, const Color * pCLUT = nullptr );

		int                 m_id = 0;

		PixelDescription	m_pixelDescription;
		int					m_pitch;
		SizeI				m_size;				// Width and height in pixels.

		ScaleMode			m_scaleMode = ScaleMode::Nearest;
		bool				m_bMipmapped = false;
		bool				m_bTiling = false;

		AccessMode			m_accessMode = AccessMode::None;
		Color *				m_pClut = nullptr;					// Pointer at color lookup table. Always 256 entries long.
		uint8_t *			m_pPixels = nullptr;				// Pointer at pixels when surface locked.
		RectI				m_lockRegion;						// Region of surface that is locked. Width/Height should be set to 0 when not locked.

		Object_p			m_pBaggage;

		// This is currently here just as metadata for WG2 compatibility, but needs to be factored in correctly in the future
		int                 m_qpixPerPoint = 4;
	};

	//____ setId() ____________________________________________________________

	void Surface::setId(int id)
	{
		m_id = id;
	}

	//____ id() _______________________________________________________________

	int Surface::id() const
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

	const Color* Surface::clut() const
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

	//____ isLocked() _________________________________________________________
	/**
	 * Check if surface is locked, e.g. its pixels are accessible.
	 *
	 * @return True if surface is locked, otherwise false.
	 */

	bool Surface::isLocked() const
	{
		return (m_accessMode != AccessMode::None);
	}

	///____ lockStatus() ______________________________________________________
	/**
	 * Check if surface is locked and if so, what kind of access is allowed.
	 *
	 * @return	ReadOnly, WriteOnly or ReadWrite if the surface is locked, otherwise None.
	 */

	AccessMode Surface::lockStatus() const
	{
		return m_accessMode;
	}

	//____ pixels() __________________________________________________________
	/**
	 * Get a pointer to the first line of raw pixels of the locked region.
	 *
	 * The pointer returned is identical to the one previously returned
	 * by the call to lock() or lockRegion().
	 *
	 * @return Pointer to the raw pixels of the locked region or null if surface is not locked.
	 */

	uint8_t * Surface::pixels() const
	{
		return m_pPixels;
	}

	//____ Surface::pitch() _______________________________________________
	/**
	 * Get the pitch of the locked region.
	 *
	 * The pitch is the distance in bytes from the start of one line of raw pixels to the start of the next one.
	 * This value must per definition be at least (bytes per raw pixel * width of locked region), but might be
	 * higher if not the whole surface is locked or if the pixel lines are padded.
	 *
	 * Please note that pitch can be negative. This allows us to handle surfaces that for technical reasons are
	 * upside-down internally in a way that is transparent to the user.
	 *
	 * @return Pitch of a pixel line of the locked region, measured in bytes, or 0 if the surface is not locked.
	 *
	 */
	int Surface::pitch() const
	{
		if( m_accessMode == AccessMode::None )
			return 0;

		return m_pitch;
	}

	//____ pixel() ____________________________________________________________
	/**
	 * Get the raw pixel value from the specified coordinate of the surface.
	 *
	 * @param x X-coordinate of pixel to get.
	 * @param y Y-coordinate of pixel to get.
	 *
	 * The coordinate specified must be within the surface boundaries. A coordinate outside
	 * the surface will result in undefined behavior.
	 *
	 * If pixel is less than
	 * 32 bits in size, it is packed into the lower bits of the return value with
	 * the rest of the bits cleared.
	 *
	 * @return Pixel value in surface's native format.
	 */
	uint32_t Surface::pixel( int x, int y ) const
	{
		return pixel( CoordI(x,y) );
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
	uint8_t Surface::alpha( int x, int y ) const
	{
		return alpha( CoordI(x,y) );
	}

	//____ regionLocked() _____________________________________________________
	/**
	 * Get the region that is locked of a locked surface.
	 *
	 * If the surface has been locked using lock(), this corresponds to the whole surface.
	 *
	 * @return The region of the surface that is locked or an empty rectangle
	 * (0,0,0,0) if surface isn't locked.
	 */
	RectI Surface::regionLocked() const
	{
		if( m_accessMode==AccessMode::None )
			return RectI();
		else
			return m_lockRegion;
	}


	//==============================================================================

} // namespace wg
#endif // WG_SURFACE_DOT_H
