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
#ifndef WG_GIZMO_PIXMAP_DOT_H
#define WG_GIZMO_PIXMAP_DOT_H


#ifndef WG_GIZMO_DOT_H
#	include <wg_gizmo.h>
#endif

#ifndef	WG_TOOLTIP_DOT_H
#	include <wg_tooltip.h>
#endif

#ifndef WG_BLOCKSET_DOT_H
#	include <wg_blockset.h>
#endif


//____ WgGizmoPixmap __________________________________________________________

class WgGizmoPixmap : public WgGizmo
{
public:
	WgGizmoPixmap();
	virtual ~WgGizmoPixmap();

	virtual const char *Type( void ) const;
	static const char * GetMyType();

	void			SetSource( const WgBlockSetPtr& pBlockSet );
	WgBlockSetPtr	GetSource() const { return m_pGfx; }

	WgSize			BestSize() const;

	DECLARE_TOOLTIP_SUPPORT();

protected:

	void	_onCloneContent( const WgGizmo * _pOrg );
	void	_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip, Uint8 _layer );
	bool	_onAlphaTest( const WgCord& ofs );
	void	_onEnable();
	void	_onDisable();

private:

	WgBlockSetPtr	m_pGfx;
};


#endif //WG_GIZMO_FILL_DOT_H
