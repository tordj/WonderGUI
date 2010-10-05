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


#ifndef WG_GIZMO_COLLECTION_DOT_H
#	include <wg_gizmo_collection.h>
#endif

#ifndef WG_GIZMO_DOT_H
#	include <wg_gizmo.h>
#endif

#ifndef WG_GFX_DOT_H
#	include <wg_gfx.h>
#endif

#define WIDGET_GIZMO_CONSTRUCTORS(_class_,_gizmoclass_) \
	_class_( WgWidget* pParent = 0 ) : Wdg_Widget<_gizmoclass_>(pParent) {}; \
	_class_( const WgRect& geometry, WgWidget * pParent = 0 ) : Wdg_Widget<_gizmoclass_>(geometry, pParent) {}; \
	_class_( WgOrigo origo, const WgRect& geometry, WgWidget * pParent = 0 ) : Wdg_Widget<_gizmoclass_>( origo, geometry, pParent ) {}; \
	_class_( WgOrigo upperLeft, Sint32 x1, Sint32 y1, WgOrigo lowerRight, Sint32 x2, Sint32 y2, WgWidget * pParent = 0 ) : Wdg_Widget<_gizmoclass_>(upperLeft,x1, y1, lowerRight, x2, y2, pParent) {}

class WgWidgetHook : public WgGizmoHook
{
public:
	WgWidgetHook( WgGizmo * pGizmo, WgGizmoCollection * pCollection, WgWidget * pWidget ) : WgGizmoHook( pGizmo,pCollection ), m_pWidget(pWidget) {}

	WgCord	Pos() const { return WgCord( m_pWidget->PosX(), m_pWidget->PosY() ); }
	WgSize	Size() const { return WgSize( m_pWidget->Width(), m_pWidget->Height() ); }
	WgRect	Geo() const { return m_pWidget->Geometry(); }

	WgCord	ScreenPos() const 
	{
		int x = 0,y = 0;
		m_pWidget->Local2abs( &x, &y );
		return WgCord(x,y); 
	}

	WgRect	ScreenGeo() const { return m_pWidget->ScreenGeometry(); }



	WgGizmoHook *	PrevHook() const
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


	WgGizmoHook *	NextHook() const
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

	WgEmitter * GetEmitter()
	{
		return m_pWidget;
	}

	inline WgWidget* GetRoot() { return m_pWidget->Root(); }			// Should in the future not return a widget, but a gizmo.

protected:

	void	RequestRender() { m_pWidget->RequestRender(); }
	void	RequestRender( const WgRect& rect ) { m_pWidget->RequestRender( rect ); }

	void	RequestResize()
	{
		// Refresh size limits

		WgSize sz = m_pGizmo->MinSize();
	
		m_pWidget->m_sizeLimit_self.minW = sz.w;
		m_pWidget->m_sizeLimit_self.minH = sz.h;

		m_pWidget->RefreshSizeLimit();
	}

	bool	RequestFocus() { return m_pWidget->GrabInputFocus(); }
	bool	ReleaseFocus() { return m_pWidget->RemoveInputFocus(); }


	WgWidget *		m_pWidget;
};



template<class T> class Wdg_Widget : public WgWidget, protected WgGizmoCollection, public T
{
public:	
	Wdg_Widget( WgWidget* pParent = 0 ) : WgWidget(pParent) { m_pHook = new WgWidgetHook(this,this,this); T::SetHook(m_pHook); }
	Wdg_Widget( const WgRect& geometry, WgWidget * pParent = 0 ) : WgWidget(geometry, pParent) {  m_pHook = new WgWidgetHook(this,this,this); T::SetHook(m_pHook); }
	Wdg_Widget( WgOrigo origo, const WgRect& geometry, WgWidget * pParent = 0 ) : WgWidget( origo, geometry, pParent ) {  m_pHook = new WgWidgetHook(this,this,this); T::SetHook(m_pHook); }
	Wdg_Widget( WgOrigo upperLeft, Sint32 x1, Sint32 y1, WgOrigo lowerRight, Sint32 x2, Sint32 y2, WgWidget * pParent = 0 ) : WgWidget(upperLeft,x1, y1, lowerRight, x2, y2, pParent) {  m_pHook = new WgWidgetHook(this,this,this); T::SetHook(m_pHook); }
	virtual ~Wdg_Widget() { T::m_pHook = 0; delete m_pHook; };

	virtual const char * Type() const { return T::Type(); }
	static const char * GetMyType() { return T::GetMyType(); }
	
	virtual WgGizmo * GetGizmo() { return this; }
	virtual const WgGizmo * GetGizmo() const { return this; }
	virtual WgGizmoHook * GetHook() const { return m_pHook; }

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


	// With wg_gizmo_widget_wrapper all widgets that are siblings behave as one collective gizmo_manager...

	virtual WgGizmoHook *	FirstHook() const
	{
		if( m_pParent )
		{
			WgWidget * p = m_pParent->FirstChild();
			
			while( p->GetHook() == 0 )
				p = p->NextSibling();

			return p->GetHook();
		}
		else
			return GetHook();	// Discards const, is ok in this case...
	}

	virtual WgGizmoHook *	LastHook() const
	{
		if( m_pParent )
		{
			WgWidget * p = m_pParent->LastChild();
			
			while( p->GetHook() == 0 )
				p = p->PrevSibling();

			return p->GetHook();
		}
		else
			return GetHook();	// Discards const, is ok in this case...
	}

	WgGizmoHook*	_firstHook() const { return FirstHook(); }
	WgGizmoHook*	_lastHook() const { return LastHook(); }


	void SetCursorStyle( WgCursorStyle style )
	{
		T::SetCursorStyle(style);
	}

	WgCursorStyle GetCursorStyle() const
	{ 
		return T::GetCursorStyle(); 
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

	WgCord		GizmoPos( const WgGizmo * pGizmo ) const
	{
		return WgCord( WgWidget::PosX(), WgWidget::PosY() );
	}


private:

	void DoMyOwnRender( const WgRect& _window, const WgRect& _clip, Uint8 _layer ) 
	{ 
		T::OnRender( WgGfx::GetDevice(), _window, _window, _clip, _layer ); 
	}

	void DoMyOwnActionRespond( WgInput::UserAction action, int button_key, const WgActionDetails& info, const WgInput& inputObj )
	{
		T::OnAction( this, action, button_key, info, inputObj );
	}

	void DoMyOwnRefresh( void )
	{
		T::OnRefresh();
	}

	void DoMyOwnUpdate( const WgUpdateInfo& _updateInfo )
	{
		T::OnUpdate( _updateInfo );
	}


	void DoMyOwnGeometryChange( WgRect& oldGeo, WgRect& newGeo ) 
	{
		T::OnNewSize( WgSize(newGeo.w, newGeo.h) );
	};

	void DoMyOwnCloning( WgWidget * _pClone, const WgWidget * _pCloneRoot, const WgWidget * _pBranchRoot )
	{
		static_cast<Wdg_Widget<T>*>(_pClone)->T::CloneContent( this );
	};

	bool DoMyOwnMarkTest( int _x, int _y )
	{
		return T::OnMarkTest( WgCord(_x,_y) );
	}

	void DoMyOwnDisOrEnable( void ) 
	{
		if( WgWidget::m_bEnabled )
			T::OnEnable();
		else
			T::OnDisable();
	
	};

	void DoMyOwnInputFocusChange( bool _bFocus )
	{
		if( _bFocus )
			T::OnGotInputFocus();
		else
			T::OnLostInputFocus();
	};

	WgWidgetHook *	m_pHook;

};


#endif //WG_GIZMO_WIDGET_WRAPPER_DOT_H
