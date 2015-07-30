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

#ifndef	WG_ICONFIELD_DOT_H
#define	WG_ICONFIELD_DOT_H

#ifndef WG_TYPES_DOT_H
#	include <wg_types.h>
#endif

#ifndef WG_FIELD_DOT_H
#	include <wg_field.h>
#endif

#ifndef WG_GEO_DOT_H
#	include <wg_geo.h>
#endif

#ifndef WG_SKIN_DOT_H
#	include <wg_skin.h>
#endif

class WgIconField;

//____ WgIconHolder ___________________________________________________________

class WgIconHolder : public WgFieldHolder
{
};

//____ WgIconField _____________________________________________________________

class WgIconField : public WgField
{
	friend class WgIconHolder;
public:
	WgIconField( WgIconHolder * pHolder );
	virtual ~WgIconField() {};


	bool			set( const WgSkinPtr& pIconGfx, WgOrigo origo = WG_WEST, WgBorder padding = WgBorder(0), 
								float _scale = 0.f, bool _bOverlap = false );
	void			clear();

	inline bool		isEmpty() const { return !m_pSkin; }

	bool			setScale( float scaleFactor );
	void			setOrigo( WgOrigo origo );
	void			setPadding( WgBorder borders );
	void			setOverlap( bool bOverlap );
	void			setSkin( const WgSkinPtr& pSkin );

	float			scale() const { return m_scale; }
	WgOrigo			origo() const { return m_origo; }
	WgBorder		padding() const { return m_padding; }
	bool			overlap() const { return m_bOverlap; }
	WgSkinPtr		skin() const { return  m_pSkin; }


	WgRect			getIconRect( const WgRect& contentRect ) const;
	WgRect			getIconRect( const WgRect& contentRect, const WgSize& iconSize ) const;
	WgRect			getTextRect( const WgRect& contentRect, const WgRect& iconRect ) const;
	void			onCloneContent( const WgIconField * _pOrg );
	WgSize			preferredSize() const;

protected:

	WgOrigo			m_origo;
	float			m_scale;					// Range: 0.f -> 1.f. 0.f = Fixed size.
	bool			m_bOverlap;
	WgBorder		m_padding;
	WgSkinPtr		m_pSkin;

};

#endif	// WG_ICONFIELD_DOT_H
