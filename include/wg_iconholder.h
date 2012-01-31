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

#ifndef	WG_ICONHOLDER_DOT_H
#define	WG_ICONHOLDER_DOT_H

#ifndef WG_TYPES_DOT_H
#	include <wg_types.h>
#endif

#ifndef WG_GEO_DOT_H
#	include <wg_geo.h>
#endif

#ifndef WG_BLOCKSET_DOT_H
#	include <wg_blockset.h>
#endif

class WgIconHolder
{
public:
	WgIconHolder();
	virtual ~WgIconHolder() {};

	bool			SetIconScale( float scaleFactor );
	void			SetIconOrientation( WgOrientation orientation );
	void			SetIconBorders( WgBorders borders );
	void			SetIconPushingText( bool bPush );

	float			IconScale() const { return m_iconScale; }
	WgOrientation	IconOrientation() const { return m_iconOrientation; }
	WgBorders		IconBorders() const { return m_iconBorders; }
	bool			IsIconPushingText() const { return m_bIconPushText; }

protected:

	virtual void	_iconModified() = 0;

	WgRect			_getIconRect( const WgRect& contentRect, const WgBlockSetPtr& pBlockSet ) const;
	WgRect			_getIconRect( const WgRect& contentRect, const WgSize& iconSize ) const;
	WgRect			_getTextRect( const WgRect& contentRect, const WgRect& iconRect ) const;
	void			_cloneContent( const WgIconHolder * _pOrg );

	WgOrientation	m_iconOrientation;
	float			m_iconScale;					// Range: 0.f -> 1.f. 0.f = Fixed size.
	bool			m_bIconPushText;
	WgBorders		m_iconBorders;



};

#endif	// WG_ICONHOLDER_DOT_H
