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

#ifndef WG_GIZMO_WIDGET_WRAPPER_DOT_H
#define WG_GIZMO_WIDGET_WRAPPER_DOT_H

#include <assert.h>

#ifndef WG_WIDGET_DOT_H
#	include <wg_widget.h>
#endif


#ifndef WG_GIZMO_CONTAINER_DOT_H
#	include <wg_gizmo_container.h>
#endif

#ifndef WG_GFX_DOT_H
#	include <wg_gfx.h>
#endif

#define WIDGET_GIZMO_CONSTRUCTORS(_class_,_gizmoclass_) \
	_class_( WgWidget* pParent = 0 ) : Wdg_Widget<_gizmoclass_>(pParent) {}; \
	_class_( const WgRect& geometry, WgWidget * pParent = 0 ) : Wdg_Widget<_gizmoclass_>(geometry, pParent) {}; \
	_class_( WgOrigo origo, const WgRect& geometry, WgWidget * pParent = 0 ) : Wdg_Widget<_gizmoclass_>( origo, geometry, pParent ) {}; \
	_class_( WgOrigo upperLeft, Sint32 x1, Sint32 y1, WgOrigo lowerRight, Sint32 x2, Sint32 y2, WgWidget * pParent = 0 ) : Wdg_Widget<_gizmoclass_>(upperLeft,x1, y1, lowerRight, x2, y2, pParent) {}


class WgWidgetHook : public WgHook
{
public:
	WgWidgetHook( WgGizmo * pGizmo, WgGizmoContainer * pParent, WgWidget * pWidget ) : WgHook(), m_pWidget(pWidget), m_pParent(pParent) { _attachGizmo(pGizmo); }
	~WgWidgetHook() { m_pGizmo = 0; }	// Prevent WgHook from deleting Gizmo (which is part of the generated Widget).
	WgCoord	Pos() const { return WgCoord( m_pWidget->PosX(), m_pWidget->PosY() ); }
	WgSize	Size() const { return WgSize( m_pWidget->Width(), m_pWidget->Height() ); }
	WgRect	Geo() const { return m_pWidget->Geometry(); }

	WgCoord	ScreenPos() const
	{
		int x = 0,y = 0;
		m_pWidget->Local2abs( &x, &y );
		return WgCoord(x,y);
	}

	WgRect	ScreenGeo() const { return m_pWidget->ScreenGeometry(); }



	WgHook *	_prevHook() const
	{
		WgWidget * p = m_pWidget->PrevSibling();

		while( p )
		{
			if( p->GetHook() )
				return p->GetHook();

			p = p->PrevSibling();
		}
		return 0;
	}


	WgHook *	_nextHook() const
	{
		WgWidget * p = m_pWidget->NextSibling();

		while( p )
		{
			if( p->GetHook() )
				return p->GetHook();

			p = p->NextSibling();
		}
		return 0;
	}

	WgGizmoContainer * _parent() const
	{
		return m_pParent;
	}

	inline WgWidget* GetRoot() { return m_pWidget->Root(); }			// Should in the future not return a widget, but a gizmo.

protected:

	void	_requestRender() { m_pWidget->RequestRender(); }
	void	_requestRender( const WgRect& rect ) { m_pWidget->RequestRender( rect ); }

	void	_requestResize()
	{
		m_pWidget->RefreshSizeLimit();
	}

	bool	_requestFocus() { return m_pWidget->GrabInputFocus(); }
	bool	_releaseFocus() { return m_pWidget->RemoveInputFocus(); }


	WgWidget *			m_pWidget;
	WgGizmoContainer *	m_pParent;
};


class WgWidgetContainer : public WgGizmoContainer
{
public:
	// With wg_gizmo_widget_wrapper all widgets that are siblings behave as one collective gizmo_manager...

	bool		DeleteChild( WgGizmo * pGizmo ) { return false; }
	WgGizmo *	ReleaseChild( WgGizmo * pGizmo ) { return 0; }

	bool		DeleteAllChildren() { return false; }
	bool		ReleaseAllChildren() { return false; }

	WgHook *	FirstHook() const
	{
		if( m_pWidget->m_pParent )
		{
			WgWidget * p = m_pWidget->m_pParent->FirstChild();

			while( p->GetHook() == 0 )
				p = p->NextSibling();

			return p->GetHook();
		}
		else
			return m_pWidget->GetHook();	// Discards const, is ok in this case...
	}

	WgHook *	LastHook() const
	{
		if( m_pWidget->m_pParent )
		{
			WgWidget * p = m_pWidget->m_pParent->LastChild();

			while( p->GetHook() == 0 )
				p = p->PrevSibling();

			return p->GetHook();
		}
		else
			return m_pWidget->GetHook();	// Discards const, is ok in this case...
	}

	WgGizmo * FindGizmo( const WgCoord& pos, WgSearchMode mode )
	{
		return 0;
	};

	bool		IsGizmo() const
	{
		WgWidget * pRealParent = m_pWidget->Parent();

		if( pRealParent && pRealParent->GetGizmo() )
			return true;
		else
			return false;
	}


	WgGizmo *	CastToGizmo() 
	{
		WgWidget * pRealParent = m_pWidget->Parent();

		if( pRealParent )
			return pRealParent->GetGizmo();
		else
			return 0;
	}





	WgWidget *		m_pWidget;

private:
	WgHook*	_firstHook() const { return FirstHook(); }
	WgHook*	_lastHook() const { return LastHook(); }

	WgHook* _firstHookWithGeo( WgRect& rect ) const { WgHook * pHook = FirstHook(); if( pHook ) rect = pHook->Geo(); return pHook; }
	WgHook* _nextHookWithGeo( WgRect& rect, WgHook * pHook ) const { pHook = pHook->Next(); if( pHook ) rect = pHook->Geo(); return pHook; }

	void	_castDirtyRect( const WgRect& geo, const WgRect& clip, WgRectLink * pDirtIn, WgRectChain* pDirtOut ) {}
	void	_renderDirtyRects( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, Uint8 _layer ) {}
	void	_clearDirtyRects() {}


};


template<class T> class Wdg_Widget : public WgWidget, public T
{
public:
	Wdg_Widget( WgWidget* pParent = 0 ) : WgWidget(pParent) { m_container.m_pWidget = this; m_container.SetRadioGroup(true); m_pHook = new WgWidgetHook(this,&m_container,this); }
	Wdg_Widget( const WgRect& geometry, WgWidget * pParent = 0 ) : WgWidget(geometry, pParent) { m_container.m_pWidget = this; m_pHook = new WgWidgetHook(this,&m_container,this); }
	Wdg_Widget( WgOrigo origo, const WgRect& geometry, WgWidget * pParent = 0 ) : WgWidget( origo, geometry, pParent ) { m_container.m_pWidget = this; m_pHook = new WgWidgetHook(this,&m_container,this); }
	Wdg_Widget( WgOrigo upperLeft, Sint32 x1, Sint32 y1, WgOrigo lowerRight, Sint32 x2, Sint32 y2, WgWidget * pParent = 0 ) : WgWidget(upperLeft,x1, y1, lowerRight, x2, y2, pParent) { m_container.m_pWidget = this; m_pHook = new WgWidgetHook(this,&m_container,this); }
	virtual ~Wdg_Widget() { T::m_pHook = 0; delete m_pHook; };

	virtual const char * Type() const { return T::Type(); }
	static const char * GetMyType() { return T::GetMyType(); }

	virtual WgGizmo * GetGizmo() { return this; }
	virtual const WgGizmo * GetGizmo() const { return this; }
	virtual WgHook * GetHook() const { return m_pHook; }

	void	Enable()
	{
		T::Enable();
		WgWidget::Enable();
	}
	void	Disable()
	{
		T::Disable();
		WgWidget::Disable();
	}

	bool	IsEnabled()
	{
		return T::IsEnabled();
	}



	inline Sint32		Id() const { return T::Id(); }
	inline void			SetId( Sint32 id ) { T::SetId(id); WgWidget::SetId(id); }

	inline void Local2abs( int * _pX, int * _pY ) const { WgWidget::Local2abs(_pX, _pY); }		// Cordinate from local cordsys to global
	inline void Abs2local( int * _pX, int * _pY ) const { WgWidget::Abs2local(_pX, _pY); } 		// Cordinate from global to local cordsys



	inline WgString	GetTooltipString() const { return T::GetTooltipString(); }


	void SetPointerStyle( WgPointerStyle style )
	{
		T::SetPointerStyle(style);
	}

	WgPointerStyle GetPointerStyle() const
	{
		return T::GetPointerStyle();
	}

	virtual bool IsInputField() const
	{
		return T::TempIsInputField();
	}

	virtual Wg_Interface_TextHolder* GetText()
	{
		return T::TempGetText();
	}




protected:
	WgWidget *	NewOfMyType() const { return new Wdg_Widget<T>; }

	WgCoord		GizmoPos( const WgGizmo * pGizmo ) const
	{
		return WgCoord( WgWidget::PosX(), WgWidget::PosY() );
	}

	WgEmitter * GetEmitter()
	{
		return this;
	}



private:

	void DoMyOwnRender( const WgRect& _window, const WgRect& _clip, Uint8 _layer )
	{
		T::_onRender( WgGfx::GetDevice(), _window, _window, _clip, _layer );
	}

	void DoMyOwnActionRespond( WgInput::UserAction action, int button_key, const WgActionDetails& info, const WgInput& inputObj )
	{
		T::_onAction( action, button_key, info, inputObj );
	}

	void DoMyOwnRefresh( void )
	{
		T::_onRefresh();
	}

	void DoMyOwnUpdate( const WgUpdateInfo& _updateInfo )
	{
		T::_onUpdate( _updateInfo );
	}


	void DoMyOwnGeometryChange( WgRect& oldGeo, WgRect& newGeo )
	{
		T::_onNewSize( WgSize(newGeo.w, newGeo.h) );
	};

	void DoMyOwnCloning( WgWidget * _pClone, const WgWidget * _pCloneRoot, const WgWidget * _pBranchRoot )
	{
		static_cast<Wdg_Widget<T>*>(_pClone)->T::CloneContent( this );
	};

	bool DoMyOwnMarkTest( int _x, int _y )
	{
		return T::_onAlphaTest( WgCoord(_x,_y) );
	}

	void DoMyOwnDisOrEnable( void )
	{
		if( WgWidget::m_bEnabled )
			T::_onEnable();
		else
			T::_onDisable();

	};

	void DoMyOwnInputFocusChange( bool _bFocus )
	{
		if( _bFocus )
			T::_onGotInputFocus();
		else
			T::_onLostInputFocus();
	};

	WgWidgetHook *	m_pHook;
	WgWidgetContainer m_container;
};


#endif //WG_GIZMO_WIDGET_WRAPPER_DOT_H
