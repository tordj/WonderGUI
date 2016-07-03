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

#ifndef	WG_ICONITEM_DOT_H
#define	WG_ICONITEM_DOT_H

#ifndef WG_TYPES_DOT_H
#	include <wg_types.h>
#endif

#ifndef WG_ITEM_DOT_H
#	include <wg_item.h>
#endif

#ifndef WG_GEO_DOT_H
#	include <wg_geo.h>
#endif

#ifndef WG_SKIN_DOT_H
#	include <wg_skin.h>
#endif

namespace wg 
{
		
	
	//____ IconItem _____________________________________________________________
	
	class IconItem : public Item
	{
	public:
		IconItem( Widget * pWidget );
		virtual ~IconItem() {};
	
	
		bool			set( const Skin_p& pIconGfx, Origo origo = Origo::West, Border padding = Border(0), 
									float _scale = 0.f, bool _bOverlap = false );
		void			clear();
	
		inline bool		isEmpty() const { return !m_pSkin; }
	
		bool			setScale( float scaleFactor );
		void			setOrigo( Origo origo );
		void			setPadding( Border borders );
		void			setOverlap( bool bOverlap );
		void			setSkin( const Skin_p& pSkin );
	
		float			scale() const { return m_scale; }
		Origo			origo() const { return m_origo; }
		Border		padding() const { return m_padding; }
		bool			overlap() const { return m_bOverlap; }
		Skin_p		skin() const { return  m_pSkin; }
	
	
		Rect			getIconRect( const Rect& contentRect ) const;
		Rect			getIconRect( const Rect& contentRect, const Size& iconSize ) const;
		Rect			getTextRect( const Rect& contentRect, const Rect& iconRect ) const;
		void			onCloneContent( const IconItem * _pOrg );
		Size			preferredSize() const;
	
	protected:
	
		Origo			m_origo;
		float			m_scale;					// Range: 0.f -> 1.f. 0.f = Fixed size.
		bool			m_bOverlap;
		Border		m_padding;
		Skin_p		m_pSkin;
	
	};
	

} // namespace wg
#endif	// WG_ICONITEM_DOT_H
