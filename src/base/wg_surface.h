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
//==============================================================================

#ifndef	WG_TYPES_DOT_H
#	include <wg_types.h>
#endif

#ifndef WG_GEO_DOT_H
#	include <wg_geo.h>
#endif

#ifndef WG_COLOR_DOT_H
#	include <wg_color.h>
#endif

#ifndef WG_POINTERS_DOT_H
#	include <wg_pointers.h>
#endif


class WgSurface;
typedef	WgStrongPtr<WgSurface,WgObjectPtr>		WgSurfacePtr;
typedef	WgWeakPtr<WgSurface,WgObjectWeakPtr>	WgSurfaceWeakPtr;

//____ WgSurface ______________________________________________________________
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


class WgSurface : public WgObject
{

public:
	bool		IsInstanceOf( const char * pClassName ) const;
	const char *ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgSurfacePtr	Cast( const WgObjectPtr& pObject );

	// Methods for reading dimensions and abilities.

	virtual	WgSize		Size() const = 0;					///< @brief Get the size of the surface.
															///<
															///< Get the width and height of the surface in a WgSize structure.
															///< @return Size of the suface measured in pixels.
	virtual	int			Width() const;						///< @brief Get the width of the surface.
	virtual	int			Height() const;						///< @brief Get the height of the surface.

	virtual bool		IsOpaque() const = 0;				///< @brief Check if surface is entirely opaque.
															///<
															///< Check if surface is entirely opaque.
															///<
															///< A surface is opaque if it contains no transparent or semi-transparent
															///< pixels at all and therefore is guaranteed to completely cover any graphics
															///< behind it. This method is called regularly in time-critical sections of
															///< the code to optimize rendering operations and therefore should not do any
															///< time consuming operations.
															///<
															///< @return True if surface is guaranteed to only contain completely opaque pixels. False if
															///< if it does contain (semi)transparent pixels or we simply just don't know.

	// Slow, simple methods for reading and parsing individual pixels.


	virtual Uint32		Pixel( WgCoord coord ) const = 0;	///< @brief Get pixel at specified coordinate.
															///<
															///< Get the raw pixel value from the specified coordinate of the surface.
															///<
															///< @param coord Coordinate of pixel to get.
															///<
															///< The coordinate must be within the surface boundaries. A coordinate outside
															///< the surface will result in undefined behavior.
															///<
															///< If pixel is less than
															///< 32 bits in size, it is packed into the lower bits of the return value with
															///< the rest of the bits cleared.
															///<
															///< @return Pixel value in surface's native format.

	inline Uint32		Pixel( int x, int y ) const;		///< @brief Get pixel at specified coordinate.

	virtual Uint8		Alpha( WgCoord coord ) const = 0;	///< @brief Get Alpha value of pixel at specified coordinate.
															///<
															///< Get the alpha value from the specified coordinate of the surface.
															///<
															///< @param coord Coordinate of pixel.
															///<
															///< Gets the alpha value of the pixel at the specified coordinate in the range of 0-255.
															///< The alpha value is by default used for opacity level,
															///< where 0 is a fully transparent pixel and 255 is opaque.
															///<
															///< The coordinate specified must be within the surface boundaries. A coordinate outside
															///< the surface will result in undefined behavior.
															///<
															///< @return Alpha value of pixel at coordinate.

	inline Uint8		Alpha( int x, int y ) const;		///< @brief Get Alpha value of pixel at specified coordinate.

	virtual	Uint32		ColorToPixel( const WgColor& col ) const;///< @brief Convert specified color to a pixel in surface's native format.
	virtual	WgColor		PixelToColor( Uint32 pixel ) const;		///< @brief Get the color and alpha values of a pixel.

	// Enums and methods for locking/unlocking of surface.

	virtual void *		Lock( WgAccessMode mode ) = 0;		///< @brief Lock the surface for direct access to pixel data.
															///<
															///< Locks the surface to gain direct access to the pixel data.
															///<
															///< @param mode	AccessMode for the locked surface, can be WG_READ_ONLY, WG_WRITE_ONLY
															///<				or WG_READ_WRITE.
															///<
															///< Locking a surface gives the user direct access to the raw pixel data of the surface.
															///< The mode of access can be limited to read-only or write-only, which might yield
															///< performance gains on some platforms as compared to read and write access.
															///<
															///< The first pixel of the first line of raw pixel data starts at the address returned
															///< by this method. The rest of the pixels for the same line follows. There might be some
															///< extra bytes between the end of one line and the beginning of the next which should
															///< neither be read or written. Use the value returned by Pitch() to properly calculate
															///< the start adress of each line.
															///<
															///< Once you are done with a surface you should release it by calling Unlock() after
															///< which no access of the raw pixel data is allowed until the surface has been locked again.
															///< Be aware that the address and pitch of the raw pixel data might change between calls to
															///< Lock().
															///<
															///< You should not use the surface as source or destination in any GfxDevice or
															///< WgSurface::CopyTo() calls while it
															///< is locked as that might result in undefined behavior on certain Surface implementations
															///< and hardware architectures.
															///<
															///< @return Pointer to start of pixel data or null if failed.

	virtual void *		LockRegion( WgAccessMode mode, const WgRect& region ) = 0; ///< @brief Lock a limited region of the surface for direct access to pixel data.
															///<
															///< Locks a limited region of the surface to gain direct access to its pixel data.
															///<
															///< @param mode	AccessMode for the locked surface, can be WG_READ_ONLY, WG_WRITE_ONLY
															///<				or WG_READ_WRITE.
															///< @param region	The rectangular region of the surface to lock.
															///<
															///< Identical to Lock() except that just a specific region of the surface may be accessed
															///< and that the pointer returned is to the first pixel of that region, not the entire surface.
															///<
															///< Use Pitch() to get the distance from the start of one pixel row of the region to the next.
															///< Use Unlock() to release the surface when you are done with it.
															///<
															///< Just locking a specific region of a surface as opposed to the whole of it might yield
															///< performance gains depending on Surface implementation and hardware architecture in use.
															///<
															///< @return Pointer to start of pixel data for specified region or null if failed.
	virtual void			Unlock() = 0;					///< @brief Unlock a locked surface.
															///<
															///< Unlocks a surface that has previously been locked by a call to Lock() or LockRegion().

	inline 	bool			IsLocked() const { return (m_accessMode != WG_NO_ACCESS); }	///< @brief Check if surface is locked.
																						///<
																						///< Check if surface is locked.
																						///< @return True if surface is locked, otherwise false.
	inline	WgAccessMode 	LockStatus() const { return m_accessMode; }	///< @brief Check if surface is locked and in what way.
																		///<
																		///< Check if surface is locked and if so, what kind of access is allowed.
																		///< @return WG_READ_ONLY, WG_WRITE_ONLY or WG_READ_WRITE if the surface is locked,
																		///< otherwise WG_NO_ACCESS.
	inline  WgRect			RegionLocked() const;						///< @brief Get the locked region of the surface.
	inline  int				Pitch() const;								///< @brief Get the pitch of the locked region.
	inline const WgPixelFormat *PixelFormat() const;					///< @brief Get the pixel format of the locked region.
	inline void *			Pixels() const { return m_pPixels; }		///< @brief Get a pointer to the raw pixels of the locked region.
																		///< Get a pointer to the first line of raw pixels of the locked region.
																		///<
																		///< The pointer returned is identical to the one previously returned
																		///< by the call to Lock() or LockRegion().
																		///< @return Pointer to the raw pixels of the locked region or
																		///<		 null if surface is not locked.


	// Methods for modifying surface content

	virtual bool		Fill( WgColor col );						///< @brief Fill surface with specified color.
	virtual bool		Fill( WgColor col, const WgRect& region );	///< @brief Fill section of surface with specified color
	virtual bool		CopyFrom( const WgSurfacePtr& pSrcSurf, const WgRect& srcRect, WgCoord dst );	///< @brief Copy block of graphics from other surface
	virtual bool		CopyFrom( const WgSurfacePtr& pSrcSurf, WgCoord dst );	///< @brief Copy other surface as a block

protected:
	WgSurface();
	virtual ~WgSurface();

	WgRect				_lockAndAdjustRegion( WgAccessMode modeNeeded, const WgRect& region );

	WgPixelFormat		m_pixelFormat;
	int					m_pitch;

	WgAccessMode		m_accessMode;
	Uint8 *				m_pPixels;			// Pointer at pixels when surface locked.
	WgRect				m_lockRegion;		// Region of surface that is locked. Width/Height should be set to 0 when not locked.
};

//____ WgSurface::Pitch() _______________________________________________
/**
 * Get the pitch of the locked region.
 *
 * The pitch is the distance in bytes from the start of one line of raw pixels to the start of the next one.
 * This value must per definition be at least (bytes per raw pixel * width of locked region), but might be
 * higher if not the whole surface is locked or if the pixel lines are padded.
 *
 * @return Pitch of a pixel line of the locked region, measured in bytes, or 0 if the surface is not locked.
 *
 **/
int WgSurface::Pitch() const
{
	if( m_accessMode == WG_NO_ACCESS )
		return 0;

	return m_pitch;
}


//____ WgSurface::PixelFormat() _______________________________________________
/**
 * Get a pointer to a WgPixelFormat struct describing the pixel format of the locked region.
 *
 * @return Pointer to a pixel format struct or nullptr if the surface isn't locked.
 **/
const WgPixelFormat *  WgSurface::PixelFormat() const
{
	if( m_accessMode == WG_NO_ACCESS )
		return 0;

	return &m_pixelFormat;
}

//____ Pixel() ______________________________________________________________
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
 **/
Uint32 WgSurface::Pixel( int x, int y ) const
{
	return Pixel( WgCoord(x,y) );
}

//____ Alpha() ____________________________________________________________
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
 **/
Uint8 WgSurface::Alpha( int x, int y ) const
{
	return Alpha( WgCoord(x,y) );
}

//____ RegionLocked() _________________________________________________________
/**
 * Get the region that is locked of a locked surface.
 *
 * If the surface has been locked using Lock(), this corresponds to the whole surface.
 *
 * @return The region of the surface that is locked or an empty rectangle
 * (0,0,0,0) if surface isn't locked.
 **/
WgRect WgSurface::RegionLocked() const
{
	if( m_accessMode==WG_NO_ACCESS )
		return WgRect();
	else
		return m_lockRegion;
}


//==============================================================================
#endif // WG_SURFACE_DOT_H
