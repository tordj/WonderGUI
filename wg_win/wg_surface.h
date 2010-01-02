#ifndef WG_SURFACE_DOT_H
#define	WG_SURFACE_DOT_H
//========================================================================================

#ifndef	WG_TYPES_DOT_H
#	include "wg_types.h"
#endif

#ifndef	WG_COLOR_DOT_H
#	include "wg_color.h"
#endif

#ifndef	WG_RECT_DOT_H
#	include "wg_rect.h"
#endif

// typedef int ETextureDataPtr;

//____ Class WgSurface _____________________________________________________________________

class WgSurface
{
// friend class GfxSystem;
friend class WgGfx;
public:
	enum BlitMode
	{
		BM_OPAQUE,
		BM_COLORKEY,
		BM_ALPHA,
		BM_COLORKEY_WITH_ALPHA
	};

//	WgSurface(ETextureDataPtr pTexture);
	WgSurface( Uint32 iWidth, Uint32 iHeight, Uint32 iBitdepth = 32 );

	~WgSurface();

	void displayFormat();
	
	inline 	Uint8	alphaValue() const;
	inline 	Uint32	colorKey() const;
	inline	Uint32	changeCnt() const;
	inline BlitMode	blitMode() const;
	inline	Uint32	width() const;
	inline	Uint32	height() const;
	inline	Uint32	pitch() const;									// Bytes per scanline.

			
  			void	setAlphaValue( Uint8 alpha );
	inline 	bool	hasAlphaChannel() const;

  inline	void	setColorKey( const WgColor& col );
  		 	void	setColorKey( Uint32 pixel );

	inline	void	incChangeCnt();
	inline	void	setChangeCnt( Uint32 );
            
	void			setBlitMode( BlitMode mode );
                                 	
	inline	void*	lock( bool bModify = false );
	inline	void	unlock( bool incChangeCnt = true );

	inline	Uint32	col2pixel( const WgColor& col ) const;
	inline	WgColor	pixel2col( Uint32 pixel ) const;
				
//	inline	ETextureDataPtr	SDL_Surf() { return m_pTexture; };
	inline	WgRect	Dimensions() const;
	
	Uint32			pixel( Uint32 x, Uint32 y ) const;		// Lock first! (for performance)
	Uint8			opacity( Uint32 x, Uint32 y ) const;	// Lock first! (for performance)
																											
protected:

//	ETextureDataPtr m_pTexture;
	
	Uint8			m_alpha;							// Surface alpha value, used for alpha-blitting if no alpha channel.
 	Uint32			m_colorKey;
	BlitMode		m_blitMode;
	Uint32			m_changeCnt;

//	Uint32			m_pitch;
//	Uint8 *			m_pLockedSurf;

	Uint32			m_width;
	Uint32			m_height;
	Uint32			m_bitdepth;
	Uint8*			m_pData;
};


//____ WgSurface::Rect() __________________________________________________________

inline WgRect WgSurface::Dimensions() const
{
	return WgRect( 0, 0, m_width, m_height );
} 

//____ WgSurface::alphaValue() ____________________________________________________________

inline Uint8 WgSurface::alphaValue() const
{
	return m_alpha;
}

//____ WgSurface::hasAlphaChannel() _______________________________________________________

inline bool	WgSurface::hasAlphaChannel() const
{
	/*
	switch(m_kImageInfo.Format)
	{
	case D3DFMT_A8R8G8B8:
	case D3DFMT_A4R4G4B4:
	case D3DFMT_A1R5G5B5:
		return true;
		break;

	case D3DFMT_X8R8G8B8:
	case D3DFMT_R5G6B5:
		return false;
		break;

	default:
		return false;
		break;
	}
	*/
	return true;
}

//____ WgSurface::setColorKey() ___________________________________________________________

inline void	WgSurface::setColorKey( const WgColor& col )
{
	setColorKey( col2pixel( col ) );
}

//____ WgSurface::colorKey() ______________________________________________________________

inline Uint32	WgSurface::colorKey() const
{
	return m_colorKey;
}

//____ WgSurface::blitMode() ______________________________________________________________

inline WgSurface::BlitMode	WgSurface::blitMode() const
{
	return m_blitMode;
}


//____ WgSurface::col2pixel() _____________________________________________________________

inline Uint32	WgSurface::col2pixel( const WgColor& col ) const
{
	// TODO: implement
	return 0;
}

//____ WgSurface::pixel2col() _____________________________________________________________

inline WgColor WgSurface::pixel2col( Uint32 pixel ) const
{
	// TODO: implement
	return 0;
}

//____ WgSurface::lock() __________________________________________________________________

inline void * WgSurface::lock( bool bModify )
{
/*
m_pLockedSurf = m_pTexture->Lock( (int*) &m_pitch, 0, bModify );
	return m_pLockedSurf;
*/

	return m_pData;
}

//____ WgSurface::unlock() __________________________________________________________________

inline void WgSurface::unlock( bool bIncChangeCnt )
{
/*
m_pTexture->Unlock();
	m_pLockedSurf = 0;
	m_pitch = 0;
*/

	if( bIncChangeCnt )
		m_changeCnt++;
}

//____ WgSurface::pitch() _________________________________________________________________

inline Uint32 WgSurface::pitch() const
{
	return m_width*m_bitdepth/8;
}


//____ WgSurface::width() _________________________________________________________________

inline Uint32 WgSurface::width() const
{
	// return m_pTexture->GetWidth();
	return m_width;
}

//____ WgSurface::height() ________________________________________________________________

inline Uint32 WgSurface::height() const
{
	// return m_pTexture->GetHeight();
	return m_height;
}

//____ WgSurface::changeCnt() _____________________________________________________________

inline Uint32 WgSurface::changeCnt() const
{
	return m_changeCnt;
}

//____ WgSurface::incChangeCnt() __________________________________________________________

inline void WgSurface::incChangeCnt()
{
	m_changeCnt++;
}

//____ WgSurface::setChangeCnt() __________________________________________________________

inline void WgSurface::setChangeCnt( Uint32 cnt )
{
	m_changeCnt = cnt;
}



//========================================================================================
#endif // WG_SURFACE_DOT_H
