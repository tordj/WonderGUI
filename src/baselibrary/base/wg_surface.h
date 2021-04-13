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
	/**
	 * @brief Simple abstract representation of a bitmap image.
	 * 
	 * @author Tord Jansson
	 * @date 26/01/21
	 * 
	 * PixelBuffer is a small, simple class for low-level access to the pixels of a Surface in
	 * a way that is independent of the underlying architecture.
	 * 
	 * PixelBuffers can not be created directly and their members are read-only. Only the 
	 * pixels (pointed to by the member pPixels) can be modified.
	 * 
	 * They are created by the Surface to which they are connected and either, depending
	 * on type of Surface, allocates a buffer for reading/writing pixels from/to the Surface
	 * OR provides direct access to the Surface pixels.
	 * 
	 * Either way you allocate a PixelBuffer by calling Surface::allocPixelBuffer() on the Surface
	 * you want to manipulate. This sets up a PixelBuffer that is mapped to a rectangular region
	 * of the Surface.
	 * 
	 * If you want to read the Surfaces' pixels you then call Surface::pushPixels() to make the Surface 
	 * copy its pixels to the buffer. This step is unnecessary if the underlying architecture 
	 * allows direct access to the pixels of the Surface, but harmless 
	 * (just returns immediately) and should always be performed to prepare the buffer for reading.
	 * 
	 * If you will just write pixels, not read, and will fill the buffer with new pixels, you can skip pushPixels()
	 * and possibly gain performance.
	 * 
	 * In order to update the Surface with the pixels in the PixelBuffer, call Surface::pullPixels() to make
	 * the Surface pull in the Pixels from the buffer.
	 * 
	 * Lastly, you need to free the memory reserved by the PixelBuffer by calling Surface::freePixelBuffer().
	 * Failing to do so will leak memory since Surfaces don't keep track of their PixelBuffers and won't delete
	 * them when destroyed.
	 * 
	 * A PixelBuffer is strongly tied to the Surface it was created from and may not be used in any call
	 * to any other Surface. So you can not push pixels from one Surface into a PixelBuffer and then pull 
	 * them into another as a way of copying content between them. Use Surface::copyFrom() for that.
	 */

	class PixelBuffer
	{
	public:
		PixelFormat		format;
		uint8_t*		pPixels;
		const Color8*	pClut;
		RectI			rect;
		int				pitch;
	};

	//____ NinePatch ___________________________________________________________
	/**
	 * @class NinePatch
	 * @author Tord Jansson
	 * @date 26/01/21
	 * 
	 * A struct that specifies a block of graphics and how it shall be copied, what
	 * parts of the block that should be stretched (if needed) and what parts should not.
	 * 
	 * A nine-patch specifies a rectangle of a Surface divided into 9 different areas:
	 * 
	 * - the four corners (northwest, northeast, southeast and south west.
	 * - the four sides between the corners (north, east, south and west).
	 * - the center.
	 * 
	 * When the block of grahics is copied to an area that is larger than the block itself,
	 * it is stretched in the following way:
	 * 
	 * - the four corners of the block are copied to the four corners of the destination (no stretching).
	 * - the north and south sides are stretched horizontally to fill the space between their corners.
	 * - the west and east sides are stretched vertically to fill the space between the corners.
	 * - the center is stretched to fill the area between the four sides.
	 * 
	 * Additionally, one extra rectangular area can be specified to not be stretched, this is what is called
	 * the 'rigid part' of the NinePatch. This is more seldom used, but is useful if you for example wants to
	 * create speech bubble like the ones used in comics. You might want the speech bubble to be able to stretch
	 * to nicely fit the text, but it also has a tail or arrow pointing to the speaker, which should not be stretched.
	 * 
	 * @var block				The rectangular area of the block of pixels to copy.
	 * @var frame				Specifies the size of the frame building up the corners and sides.
	 * @var rigidPartXOfs		Start of the rigid part along the X-axis, from the beginning of the block (block.x).
	 * @var rigidPartXLength	Length of the rigid part along the X-axis.
	 * @var rigidPartXSections	Top, center, bottom or any combination of these, OR:ed together. Specifying what
	 * 							sections should be rigid horizontally.
	 * @var rigidPartYOfs		Start of the rigid part along the Y-axis, from the beginning of the block (block.y).
	 * @var rigidPartYLength	Length of the rigid part along the Y-axis.
	 * @var rigidPartYSections	Left, center, right or any combination of these, OR:ed together. Specifying what
	 * 							sections should be rigid horizontally.
	 * 
	 */

	struct NinePatch
	{
		Rect		block;
		Border		frame;

		pts			rigidPartXOfs = 0;
		pts			rigidPartXLength = 0;
		YSections	rigidPartXSections = YSections::None;

		pts			rigidPartYOfs = 0;
		pts			rigidPartYLength = 0;
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

		inline SizeI		pixelSize() const;						///< @brief Get the size in pixels of the surface.
		inline	int			pixelWidth() const;						///< @brief Get the width of the surface.
		inline	int			pixelHeight() const;						///< @brief Get the height of the surface.

		inline Size			pointSize() const;
		inline pts			pointWidth() const;
		inline pts			pointHeight() const;

		bool                setScale( int scale );
		inline int			scale() const;


		//.____ Appearance ____________________________________________________

		virtual void		setScaleMode( ScaleMode mode );
		inline ScaleMode	scaleMode() const;

		virtual bool		setTiling(bool bTiling);
		inline bool			isTiling() const { return m_bTiling; }
		
		inline bool			isMipmapped() const;

		//.____ Content _______________________________________________________

		virtual int			alpha( CoordSPX coord ) = 0;	///< @brief Get Alpha value of subpixel at specified coordinate.

		virtual	uint32_t	colorToPixel( const HiColor& col ) const;		///< @brief Convert specified color to a pixel in surface's native format.
		virtual	HiColor		pixelToColor( uint32_t pixel ) const;		///< @brief Get the color and alpha values of a pixel.

		inline const Color8* clut() const;

		inline const PixelDescription*	pixelDescription() const; ///< @brief Get the pixel description for the surface.
		inline PixelFormat	pixelFormat() const;
		inline int			pixelBytes() const;

		virtual bool		isOpaque() const = 0;				///< @brief Check if surface is entirely opaque.

		//.____ Control _______________________________________________________

		inline const PixelBuffer allocPixelBuffer();

		virtual const PixelBuffer allocPixelBuffer(const RectI& rect) = 0;

        inline bool			pushPixels(const PixelBuffer& buffer);
		virtual bool		pushPixels(const PixelBuffer& buffer, const RectI& bufferRect) = 0;

        inline void			pullPixels(const PixelBuffer& buffer);
		virtual void		pullPixels(const PixelBuffer& buffer, const RectI& bufferRect) = 0;

		virtual void		freePixelBuffer(const PixelBuffer& buffer) = 0;


		//.____  Rendering ____________________________________________________

		virtual bool		fill( HiColor col );								///< @brief Fill surface with specified color.
		virtual bool		fill( HiColor col, const RectI& region );			///< @brief Fill section of surface with specified color
		virtual bool		copyFrom( Surface * pSrcSurf, const RectI& srcRect, CoordI dst );	///< @brief Copy block of graphics from other surface
		virtual bool		copyFrom( Surface * pSrcSurf, CoordI dst );		///< @brief Copy other surface as a block

		//.____ Misc _________________________________________________________

		inline void			setBaggage(Object * pBaggage);
		inline Object_p		baggage() const;

	protected:
		Surface( int flags );
		virtual ~Surface();

		static const uint8_t *	s_pixelConvTabs[9];

		bool 				_copyFrom( const PixelDescription * pSrcFormat, uint8_t * pSrcPixels, int srcPitch, const RectI& srcRect, const RectI& dstRect, const Color8 * pCLUT = nullptr );

		int                 m_id = 0;

        int             	m_flags;                            // The flags provided to the constructor.
		int					m_scale = 64;

		PixelDescription	m_pixelDescription;
		SizeI				m_size;								// Width and height in pixels.

		ScaleMode			m_scaleMode = ScaleMode::Nearest;
		bool				m_bMipmapped = false;
		bool				m_bTiling = false;

		Color8 *			m_pClut = nullptr;					// Pointer at color lookup table. Always 256 entries long.

		Object_p			m_pBaggage;

	};

	//____ setIdentity() ____________________________________________________________
	/**
	 * @brief Set the ID of this Surface
	 * 
	 * @param id	An integer to be saved in the Surface as its ID.
	 * 
	 * The ID is not used internally by WonderGUI and doesn't affect anything. It just
	 * provides a simple way for you to tag a Surface for later identification.
	 */

	void Surface::setIdentity(int id)
	{
		m_id = id;
	}

	//____ identity() _______________________________________________________________
	/**
	 * @brief Get the ID of this Surface
	 * 
	 * @return ID of this Surface. Default ID for all surfaces is 0.
	 * 
	 * The ID is not used internally by WonderGUI and doesn't affect anything. It just
	 * provides a simple way for you to tag a Surface for later identification.
	 */

	int Surface::identity() const
	{
		return m_id;
	}

	//____ pixelSize() _____________________________________________________________
	/**
	 * Get the width and height of the surface in a SizeI structure.
	 *
	 * @return Size of the suface measured in pixels.
	 */
	SizeI Surface::pixelSize() const
	{
		return m_size;
	}

	//____ pixelWidth() ________________________________________________________________
	/**
	 * Get the width of the surface.
	 *
	 * @return The width of the surface, measured in pixels.
	 **/
	int Surface::pixelWidth() const
	{
		return m_size.w;
	}

	//____ pixelHeight() _______________________________________________________________
	/**
	 * Get the height of the surface.
	 *
	 * @return The height of the surface, measured in pixels.
	 **/
	int Surface::pixelHeight() const
	{
		return m_size.h;
	}

	//____ pointSize() _____________________________________________________________
	/**
	 * Get the width and height of the surface in a Size structure.
	 *
	 * @return Size of the suface measured in points.
	 */
	Size Surface::pointSize() const
	{
		return Size(m_size*64)/m_scale;
	}

	//____ pointWidth() ________________________________________________________________
	/**
	 * Get the width of the surface.
	 *
	 * @return The width of the surface, measured in points.
	 **/
	pts Surface::pointWidth() const
	{
		return pts(m_size.w*64)/m_scale;
	}

	//____ pointHeight() _______________________________________________________________
	/**
	 * Get the height of the surface.
	 *
	 * @return The height of the surface, measured in points.
	 **/
	pts Surface::pointHeight() const
	{
		return pts(m_size.h*64)/m_scale;
	}


	//____ scale() ____________________________________________________________

	int Surface::scale() const
	{
		return m_scale;
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

	//____ allocPixelBuffer() __________________________________________________
	/**
	 * @brief 	Allocate a buffer to access the pixels of the full surface
	 * @return 	A PixelBuffer providing info about the buffer, as well as working as a handle for
	 * 			future references.
	 * 
	 * Allocates a buffer that provides acces to the pixels of the full surface.
	 * 
	 * The buffer should be expected to be uninitialized, but might already contain the
	 * content of the Surface depending on Surface implementation. For example is the
	 * buffer of a SoftSurface just a pointer into the pixels of the surface, providing
	 * direct access, while the OpenGL and Metal surfaces are uninitialized since we don't
	 * have direct access to the texture pixels.
	 * 
	 * To fill the buffer with the content of the surface, call pushPixels(). To copy the
	 * content of the buffer to the surface, use pullPixels(). These calls will do the actual
	 * pixel copying for an OpenGL and Metal surface, while just immediately return for a SoftSurface.
	 * 
	 * Once you are done with your buffer, call freePixelBuffer() to return the memory allocated.
	 * 
	 */
	
	const PixelBuffer Surface::allocPixelBuffer() 
	{ 
		return allocPixelBuffer(m_size); 
	}

	//____ pushPixels() _____________________________________________________
	/**
	 * @brief Copy pixels from Surface to PixelBuffer.
	 * 
	 * Copy pixels from Surface to buffer.
	 * 
	 * @param buffer		PixelBuffer struct describing the buffer to copy pixels to.
	 * @return 				True if operation could be performed.
	 * 
	 * Fills the buffer with the pixels from the surface for the area the buffer maps.
	 */

    bool Surface::pushPixels(const PixelBuffer& buffer)
	{ 
		return pushPixels(buffer, buffer.rect.size()); 
	}

	//____ pullPixels() ______________________________________________________
	/**
	 * @brief Copy pixels from PixelBuffer to Surface.
	 * 
	 * Copy pixels from buffer to Surface.
	 * 
	 * @param buffer		PixelBuffer struct describing the buffer containg the pixels to copy.
	 * @return 				True if operation could be performed.
	 * 
	 * Updates the area of the Surface that the PixelBuffer maps to with the pixels from the buffer.
	 */

    void Surface::pullPixels(const PixelBuffer& buffer)
	{ 
		 pullPixels(buffer, buffer.rect.size()); 
	}

	//____ setBaggage() _______________________________________________________
	/**
	 * @brief Attach an Object as baggage.
	 * 
	 * @param pBaggage Pointer at the object to become the baggage.
	 * 
	 * A Surface can "carry around" one Object as its baggage. Typical baggage would
	 * be a DataSet or Blob containing meta data about the Surface that might be
	 * relevant to your application.
	 * 
	 * The baggage is irrelevant to and ignored by WonderGUI.
	 */

	void Surface::setBaggage(Object * pBaggage)
	{
		m_pBaggage = pBaggage;
	}


	//_____ baggage() _________________________________________________________
	/**
	 * @brief Get the baggage for this Surface.
	 * 
	 * @return The Object assigned as baggage or nullptr if none.
	 * 
	 * A Surface can "carry around" one Object as its baggage. Typical baggage would
	 * be a DataSet or Blob containing meta data about the Surface that might be
	 * relevant to your application.
	 * 
	 * The baggage is irrelevant to and ignored by WonderGUI.
	 * 
	 */

	Object_p Surface::baggage() const
	{
		return m_pBaggage;
	}



	//==============================================================================

} // namespace wg
#endif // WG_SURFACE_DOT_H
