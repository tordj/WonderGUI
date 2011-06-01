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

#ifndef WG_GIZMO_ITEM_WRAPPER_DOT_H
#define WG_GIZMO_ITEM_WRAPPER_DOT_H

#include <wg_item.h>
#include <wg_gizmo.h>

template<class T> class WgGizmoItem : public WgItem , protected WgGizmoCollection, public T
{
public:	
	WgGizmoItem( void ) {};
	WgGizmoItem( Uint32 id ) : WgItem(id) { T::m_id = id; }
	~WgGizmoItem() {};

	const char *	Type() const { return T::Type(); }
	static const char * GetMyType() { return T::GetMyType(); }

	void SetId(Uint32 id)
	{ 
		WgItem::m_id = id; 
		T::m_id = id; 
	}

	void Enable() 
	{ 
		WgItem::Enable(); 
		WgGizmo::Enable(); 
	}

	void Disable() 
	{ 
		WgItem::Disable(); 
		WgGizmo::Disable(); 
	}

	WgString GetTooltipString() const	
	{ 
		return T::m_tooltip; 
	}



protected:

	// GizmoManager-interface

	bool		GizmoGrabFocus( WgGizmo * pGizmo ) { return WgItem::GrabInputFocus(); }
	bool		GizmoReleaseFocus( WgGizmo * pGizmo ) { return WgItem::RemoveInputFocus(); }

	WgCord		GizmoPos( const WgGizmo * pGizmo ) const { return WgCord(0,0); }
	WgCord		GizmoScreenPos( const WgGizmo * pGizmo ) const { return WgCord(0,0); }

	WgSize GizmoSize( const WgGizmo * pGizmo ) const 
	{ 
		return WgSize( WgItem::m_width, WgItem::m_height ); 
	}

	void GizmoRequestRender( const WgGizmo * pGizmo ) 
	{ 
		WgItem::Modified(0,0);
	}
	void GizmoRequestRender( const WgGizmo * pGizmo, const WgRect& rect ) 
	{ 
		WgItem::Modified(0,0);
	}

	void GizmoRequestResize( const WgGizmo * pGizmo )
	{
		WgItem::Modified( pGizmo->BestSize() - WgItem::m_width, pGizmo->BestSize() - WgItem::m_height );
	}

	//


	void _onGotInputFocus() 
	{
		T::_onGotInputFocus();
	}

	void _onLostInputFocus() 
	{
		T::_onLostInputFocus();
	}



	void AdaptToWidth( Uint32 displayed_width ) 
	{ 
		T::_onNewSize( WgSize( displayed_width, WgItem::m_height) ); 
		WgItem::m_width = displayed_width;
	}

	void AdaptToHeight( Uint32 displayed_height ) 
	{ 
		T::_onNewSize( WgSize( WgItem::m_width, displayed_height ) ) ;
		WgItem::m_height = displayed_height;
	}

	void ActionRespond( WgEmitter * pEmitter, WgInput::UserAction action, int button_key, const WgActionDetails& info, const WgInput& inputObj )
	{
		if( action == WgInput::KEY_PRESS )
			int x = 0;

		T::_onAction( action, button_key, info, inputObj );
	}

 	void Render( const WgRect& _window, const WgRect& _clip )
	{
		T::_onRender( WgGfx::GetDevice(), _window, _window, _clip, 0 );
	}

	void Refresh( void )
	{
		T::_onRefresh();
	}

	WgItem* Clone( WgItem * _pClone )
	{
		WgItem::Clone( _pClone );

		if( _pClone->Type() == Type() )
			((WgGizmoItem*)_pClone)->_onCloneContent(this);

		return _pClone;
	}

	WgItem*	GetMarkedItem( Uint32 _x, Uint32 _y )
	{
		if( T::_onAlphaTest( WgCord( _x, _y ) ) )
			return this;
			 
		return 0;
	}

	virtual void Enable( bool bEnable )
	{
		if( bEnable )
			T::Enable();
		else
			T::Disable();
	}; 



};


#endif //WG_GIZMO_ITEM_WRAPPER_DOT_H