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

#ifndef	WG_CARET_DOT_H
#define WG_CARET_DOT_H

#ifndef	WG_TYPES_DOT_H
#	include <wg_types.h>
#endif

#ifndef WG_GEO_DOT_H
#	include <wg_geo.h>
#endif

#ifndef WG_GFXANIM_DOT_H
#	include <wg_gfxanim.h>
#endif

//class WgGfxAnim;
class WgGlyphset;

class WgCaret;
typedef	WgStrongPtr<WgCaret,WgObjectPtr>		WgCaretPtr;
typedef	WgWeakPtr<WgCaret,WgObjectWeakPtr>		WgCaretWeakPtr;

//____ WgCaret _______________________________________________________________

class WgCaret : public WgObject
{
	friend class WgGlyphset;

public:
	static WgCaretPtr	Create() { return WgCaretPtr(new WgCaret()); }
	static WgCaretPtr	Create( const WgCaretPtr& in ) { return WgCaretPtr(new WgCaret(in.RawPtr())); }

	bool		IsInstanceOf( const char * pClassName ) const;
	const char *ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgCaretPtr	Cast( const WgObjectPtr& pObject );


	enum Mode
	{
		EOL = 0,
		INS,
		OVR
	};
	
	enum BlitMode
	{
		NORMAL,
		TINTED,
		INVERT_BG
	};


	bool				SetMode( Mode m, const WgGfxAnimPtr& pAnim, WgCoord bearing = WgCoord(), int advance = 0, float size_ratio = 1.f  );

	void				SetScaleWidth( Mode m, bool bScaleWidth );

	void				SetBearing( Mode m, WgCoord bearing );
	void				SetAdvance( Mode m, int advance );
	void				SetAnim( Mode m, const WgGfxAnimPtr& pAnim );

	void				SetSizeRatio( Mode m, float ratio );

	void				SetBlitMode( BlitMode mode );
	BlitMode			GetBlitMode() const { return m_blitMode; }

	int					BearingX( Mode m ) const { return m_bearing[m].x; };
	int					BearingY( Mode m ) const { return m_bearing[m].y; };
	WgCoord				Bearing( Mode m ) const { return m_bearing[m]; }
	int					Advance( Mode m ) const { return m_advance[m]; };
	int					Width( Mode m ) const { return m_pAnim[m]->Size().w; }
	WgGfxAnimPtr 		Anim( Mode m ) const { return m_pAnim[m]; };
	float				SizeRatio(Mode m ) const { return m_sizeRatio[m]; }
	bool				ScaleWidth(Mode m ) const { return m_scaleWidth[m]; }
	

private:
	WgCaret();
	WgCaret( WgCaret * pIn );

	enum { N_MODES = 3 };

	WgGfxAnimPtr		m_pAnim[N_MODES];
	WgCoord				m_bearing[N_MODES];
	int					m_advance[N_MODES];
	bool				m_scaleWidth[N_MODES];

	float				m_sizeRatio[N_MODES];			// ratio <= 1.f. Cursors height relative fonts lineheight.

	BlitMode			m_blitMode;
};





#endif // WG_CARET_DOT_H
