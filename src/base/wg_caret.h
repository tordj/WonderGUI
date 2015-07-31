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

namespace wg 
{
	
	//class GfxAnim;
	class Glyphset;
	
	class Caret;
	typedef	WgStrongPtr<Caret,Object_p>		Caret_p;
	typedef	WgWeakPtr<Caret,Object_wp>		Caret_wp;
	
	//____ Caret _______________________________________________________________
	
	class Caret : public Object
	{
		friend class Glyphset;
	
	public:
		static Caret_p	create() { return Caret_p(new Caret()); }
		static Caret_p	create( const Caret_p& in ) { return Caret_p(new Caret(in.rawPtr())); }
	
		bool		isInstanceOf( const char * pClassName ) const;
		const char *className( void ) const;
		static const char	CLASSNAME[];
		static Caret_p	cast( const Object_p& pObject );
	
	
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
	
	
		bool				setMode( Mode m, const GfxAnim_p& pAnim, Coord bearing = Coord(), int advance = 0, float size_ratio = 1.f  );
	
		void				setScaleWidth( Mode m, bool bScaleWidth );
	
		void				setBearing( Mode m, Coord bearing );
		void				setAdvance( Mode m, int advance );
		void				setAnim( Mode m, const GfxAnim_p& pAnim );
	
		void				setSizeRatio( Mode m, float ratio );
	
		void				setBlitMode( BlitMode mode );
		BlitMode			getBlitMode() const { return m_blitMode; }
	
		int					bearingX( Mode m ) const { return m_bearing[m].x; };
		int					bearingY( Mode m ) const { return m_bearing[m].y; };
		Coord				bearing( Mode m ) const { return m_bearing[m]; }
		int					advance( Mode m ) const { return m_advance[m]; };
		int					width( Mode m ) const { return m_pAnim[m]->size().w; }
		GfxAnim_p 		anim( Mode m ) const { return m_pAnim[m]; };
		float				sizeRatio(Mode m ) const { return m_sizeRatio[m]; }
		bool				scaleWidth(Mode m ) const { return m_scaleWidth[m]; }
		
	
	private:
		Caret();
		Caret( Caret * pIn );
	
		enum { N_MODES = 3 };
	
		GfxAnim_p		m_pAnim[N_MODES];
		Coord				m_bearing[N_MODES];
		int					m_advance[N_MODES];
		bool				m_scaleWidth[N_MODES];
	
		float				m_sizeRatio[N_MODES];			// ratio <= 1.f. Cursors height relative fonts lineheight.
	
		BlitMode			m_blitMode;
	};
	
	
	
	
	

} // namespace wg
#endif // WG_CARET_DOT_H
