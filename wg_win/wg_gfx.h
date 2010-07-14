#ifndef	WG_GFX_DOT_H
#define WG_GFX_DOT_H

#ifndef NULL
#define NULL (0L)
#endif

#ifndef	WG_TYPES_DOT_H
#	include "wg_types.h"
#endif

#ifndef	WG_RECT_DOT_H
#	include "wg_rect.h"
#endif

#ifndef	WG_ORIGO_DOT_H
#	include "wg_origo.h"
#endif

#ifndef	WG_TEXTTOOL_DOT_H
#	include "wg_texttool.h"
#endif


//#include "../mig/Sdk/src/ETypes/EColor.h"

class WgColor;
class WgSurface;
class WgText;
class WgFont;
class WgFontSet;
class WgCursorInstance;

//____ WgGfx __________________________________________________________________

class WgGfx
{
	friend class WgSurface;

public:

	// BlendModes control how blits and fills are blended against the background.

	enum	BlendMode
	{
		WG_BLENDMODE_OPAQUE,			///< Completely opaque blitting, ignoring alpha of source and tint-color.
		WG_BLENDMODE_BLEND,				///< Normal mode, alpha of source and tint-color is taken into account.
		WG_BLENDMODE_ADD,				///< RGBA Additive.
		WG_BLENDMODE_MULTIPLY,			///< RGBA Multiply.
		WG_BLENDMODE_OVERLAYTINT
	};

	static bool			setCanvas( WgSurface * pSurf );
	static WgSurface * 	canvas( void );

	static void		setTintColor( WgColor color );
	static bool		setBlendMode( BlendMode blendMode );

	static inline const WgColor&	tintColor() { return m_wgTintColor; }
	static inline BlendMode blendMode() { return m_wgBlendMode; }

	static void		fillRect( const WgRect& rect, const WgColor& col );
	static void		clipFillRect( const WgRect& clip, const WgRect& rect, const WgColor& col );


	static void		blit( const WgSurface* src );
	static void		blit( const WgSurface* src, Sint32 dx, Sint32 dy );
	static void		blit( const WgSurface* src, const WgRect& srcrect, Sint32 dx, Sint32 dy  );	

	static void		clipBlit( const WgRect& clip, const WgSurface* src );
	static void		clipBlit( const WgRect& clip, const WgSurface* src, Sint32 dx, Sint32 dy );
	static void		clipBlit( const WgRect& clip, const WgSurface* src, const WgRect& srcrect, Sint32 dx, Sint32 dy  );

	static void		stretchBlit( const WgSurface* _pSrc );
	static void		stretchBlit( const WgSurface * pSrc, const WgRect& dest );
	static void		stretchBlit( const WgSurface * pSrc, const WgRect& src, const WgRect& dest );

	static void		clipStretchBlit( const WgRect& clip, const WgSurface* _pSrc );
	static void		clipStretchBlit( const WgRect& clip, const WgSurface * pSrc, const WgRect& dest );
	static void		clipStretchBlit( const WgRect& clip, const WgSurface * pSrc, const WgRect& src, const WgRect& dest );

	// High-level blit methods

	static void		clipTileBlit( const WgRect& clip, const WgSurface* src, const WgRect& srcrect, const WgRect& dest );

	static void		clipBlitRow( const WgRect& clip, const WgSurface * src, const WgRect& srcrect, 
								 Uint32 tileOfs, Uint32 tileLen, Sint32 dx, Sint32 dy, Uint32 len, bool bStretch = false );

	static void		clipBlitColumn( const WgRect& clip, const WgSurface * src, const WgRect& srcrect, 
									Uint32 tileOfs, Uint32 tileLen, Sint32 dx, Sint32 dy, Uint32 len, bool bStretch = false );

	static void		clipBlitFramedTiles( const WgRect& clip, const WgSurface* pSrc, const WgRect& srcOuterFrame, 
										 const WgRect& srcInnerFrame, const WgRect& dest, bool bStretch = false );

	// High-level print methods

	static void		printText( const WgText * pText, const WgRect& dest );
	static void		clipPrintText( const WgRect& clip, const WgText * pText, const WgRect& dest );

	static void		printTextWithCursor( const WgText * pText, const WgCursorInstance& cursor, const WgRect& dest );
	static void		clipPrintTextWithCursor( const WgRect& clip, const WgText * pText, const WgCursorInstance& cursor, const WgRect& dest );

	// Low-level print methods

	static void		printLine( WgTextTool::TextParam& param, int x, int y, const Uint16 * pString, Uint32 maxChars = 0xFFFF );
	static void		clipPrintLine( const WgRect& clip, WgTextTool::TextParam& param, int x, int y, 
										const Uint16 * pString, Uint32 maxChars = 0xFFFF );

private:

	static WgColor		m_wgTintColor;		// Tint color in WonderGUI format.
	static BlendMode	m_wgBlendMode;		// BlendMode in WonderGUI format.

	static WgSurface *	m_pCanvas;
//	static EColor		m_tintColor;		// Tint color in native format.
//	static int			m_blendMode;		// BlendMode in native format.

};



//____ WgGfx::clipFillRect() __________________________________________________

inline void WgGfx::clipFillRect( const WgRect& _clip, const WgRect& _rect, const WgColor& _col )
{
	fillRect( WgRect( _clip, _rect ), _col );
}


#endif // WG_GFX_DOT_H
