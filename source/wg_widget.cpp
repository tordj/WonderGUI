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

#include	<wg_widget.h>
#include	<wdg_root.h>
#include	<wg_origo.h>

#include	<assert.h>

using namespace WgSignal;


const	static char	 Widget_String[] = {"Unspecified"};


//____ WgWidget() _____________________________________________________________

WgWidget::WgWidget( WgWidget* pParent )
{
	Init();
	SetParent( pParent );
	if( pParent )
		SetGeometry( WgOrigo::topLeft(), 0, 0, WgOrigo::bottomRight(), 0, 0 );
}

WgWidget::WgWidget( const WgRect& geometry, WgWidget * pParent )
{
	Init();
	SetParent( pParent );
	SetGeometry( WgOrigo::topLeft(), geometry );
}

WgWidget::WgWidget( WgOrigo origo, const WgRect& geometry, WgWidget * pParent )
{
	Init();
	SetParent( pParent );
	SetGeometry( origo, geometry );
}

WgWidget::WgWidget( WgOrigo topLeft, Sint32 x1, Sint32 y1, WgOrigo bottomRight,
										Sint32 x2, Sint32 y2, WgWidget * pParent )
{
	Init();
	SetParent( pParent );
	SetGeometry( topLeft, x1, y1, bottomRight, x2, y2 );
}


//____ ~WgWidget() ____________________________________________________________

WgWidget::~WgWidget( void )
{
	WgWidget * pChild = m_pFirstChild;
	while( pChild != 0 )
	{
		WgWidget * pTmp = pChild;
		pChild = pChild->m_pNextSibling;
		delete pTmp;
	}


	SetParent( 0 );
}

//____ Init() _________________________________________________________________

void WgWidget::Init()
{
	m_pPrevSibling 		= 0;
	m_pNextSibling 		= 0;
	m_pParent 			= 0;
	m_pFirstChild 		= 0;
	m_pLastChild 		= 0;

	m_geo.x				= 0;
	m_geo.y				= 0;
	m_geo.w 			= 4;
	m_geo.h 			= 4;
	m_x1				= 0;
	m_y1				= 0;
	m_x2				= 4;
	m_y2				= 4;

	m_bOpaque 			= false;					// Completely opaque rectangle is default for widgets
	m_bRenderedHere 	= false;				// Not rendered yet.
	m_pointerMask		= POINTER_SOURCE_ALPHA;
	m_bRendersAll 		= false;
	m_bRenderOne 		= false;
	m_bModal 			= false;
	m_bEnabled 			= true;
	m_bHidden			= false;
	m_bConstrainChildren= true;
//	m_origo						= WgOrigo::topLeft();  Already top-left by default.

	m_actionFilterCopy	= 0;
	m_actionFilterBlock = 0;


	m_sizeLimit.minW 	= 1;
	m_sizeLimit.maxW 	= 10000;
	m_sizeLimit.minH 	= 1;
	m_sizeLimit.maxH 	= 10000;

	m_sizeLimit_self 	= m_sizeLimit;
	m_sizeLimit_tree 	= m_sizeLimit;
	m_sizeLimit_user 	= m_sizeLimit;

	m_id 				= 0;
	m_layer				= WG_DEFAULT_LAYER;
	m_tooltipDelay		= 500; // default 500 msec tooltip delay

	m_cursorStyle		= WG_CURSOR_DEFAULT;
}

//____ Type() _________________________________________________________________
/**
	Returns the type of this widget, which is a zero-terminated 7-bit ASCII-string.
*/

const char * WgWidget::Type( void ) const
{
	return GetMyType();
}

const char * WgWidget::GetMyType()
{
	return Widget_String;
}


//____ SetGeometry(1) _________________________________________________________
/**
	Sets the geometry for this widget with separate orgios for top-left and
	bottom-right corners, causing the widget to resize when parent is resized.

	Please note that by doing this you might also indirectly put constraints on
	parents minimum and maximum size, since a child will prevent its parent from
	getting a size that inidrectly would give the child a size or position it
	isn't allowed to have.

	@param topLeft			Origo for top-left corner of this widget. Hotspot ignored.
	@param x1						Pixeloffset from origo along X-axis for top-left corner.
	@param y1						Pixeloffset from origo along Y-axis for top-left corner.
	@param bottomRight	Origo for bottom-right corner of this widget. Hotspot ignored.
	@param x2						Pixeloffset from origo along X-axis for bottom-right corner.
	@param y2						Pixleoffset from origo along Y-axis for bottom-right corner.

	Bottom-right origo must obviously not be above or left of the top-left origo.

	In case of a totally illegal geometry or a geometry that doesn't fit inside
	parents current size, the pixeloffsets are changed to make it fit in.

	@return False if the origo combination is illegal (nothing changed in widgets
					geometry) or if pixeloffsets had to be modified (geometry changed).

*/

bool WgWidget::SetGeometry( WgOrigo _topLeft, Sint32 _x1, Sint32 _y1, WgOrigo _bottomRight, Sint32 _x2, Sint32 _y2 )
{
	if( _topLeft.anchorX() > _bottomRight.anchorX() || _topLeft.anchorY() > _bottomRight.anchorY() )
		return false;

	m_origo1 = WgOrigo::specific(_topLeft.anchorX(), _topLeft.anchorY(), 0, 0);
	m_origo2 = WgOrigo::specific(_bottomRight.anchorX(), _bottomRight.anchorY(), 0, 0);

	m_x1 = _x1;
	m_y1 = _y1;
	m_x2 = _x2;
	m_y2 = _y2;

	return UpdateGeometry( true );
}

//____ SetGeometry(2) _________________________________________________________
/**
	Sets the geometry of this widget.

	@param	origo			Origo for this widget.
	@param	geometry	Geometry of this widget, where x and y position are relative
										to origo.

	If the specified geometry doesn't fit inside the parent, the size of the
	widget is first adjusted and secondly the position. Origo is always accepted
	as it is.

	@return False if the specified geometry wasn't allowed and had to be tweaked.
*/

bool WgWidget::SetGeometry( WgOrigo _origo, const WgRect& _geometry )
{
	m_origo1	= _origo;
	m_origo2	= _origo;

	m_x1 = _geometry.x;
	m_y1 = _geometry.y;
	m_x2 = _geometry.x + _geometry.w;
	m_y2 = _geometry.y + _geometry.h;

	return UpdateGeometry( true );
}

//____ CloneGeometry() ________________________________________________________
/**
	Clones the geometry from another widget.

	@param	pCloneFrom		The widget to clone the geometry from.

	If the specified geometry doesn't fit inside the parent, the size of the
	widget is first adjusted and secondly the position. Origo is always accepted
	as it is.

	@return False if the specified geometry wasn't allowed and had to be tweaked.
*/

//____ CloneGeometry() ________________________________________________________
bool WgWidget::CloneGeometry( const WgWidget* pCloneFrom )
{
	m_origo1	= pCloneFrom->m_origo1;
	m_origo2	= pCloneFrom->m_origo2;

	m_x1 = pCloneFrom->m_x1;
	m_y1 = pCloneFrom->m_y1;
	m_x2 = pCloneFrom->m_x2;
	m_y2 = pCloneFrom->m_y2;

	return UpdateGeometry( true );
}


//____ RefreshBranch() ________________________________________________________
/**
	Forces this widget and all its sub-widgets to check dependencies against
	Surfaces, fonts etc and redraw graphics if something has changed.
*/

void WgWidget::RefreshBranch( void )
{
	DoMyOwnRefresh();

	for( WgWidget * pChild = m_pFirstChild ; pChild != 0 ; pChild = pChild->m_pNextSibling )
		RefreshBranch();
}


//____ SetParent() ____________________________________________________________
/**
	Makes this widget become a child of the specified parent. New children are put
	on top of older siblings. This automatically moves a whole branch if this
	widget has children.

	Widget might be resized or repositioned compared to what was earlier specified
	if new parents size doesn't allow widgets geometry. Widgets are at first
	repositioned and only resized if repositioning isn't enough.
*/

bool WgWidget::SetParent( WgWidget * _pNewParent, WgWidget * _pBelow )
{
	if( _pNewParent == m_pParent )
		return true;

	// Check so we don't set ourself or one of our children as parent

	WgWidget * p = _pNewParent;
	while( p )
	{
		if( p == this )
			return false;
		p = p->Parent();
	}

	// Check so that specified _pBelow has _pNewParent as its parent and has same modality.

	if( _pBelow )
	{
		if( !_pNewParent || _pBelow->m_pParent != _pNewParent || _pBelow->m_bModal != m_bModal )
			return false;
	}

	// Check so we don't have any size constraints that would fail.

	if( _pNewParent )
	{
		if( m_sizeLimit.minW > _pNewParent->m_geo.w || m_sizeLimit.minH > _pNewParent->m_geo.h )
		{
//			assert( false );
			return false;
		}
	}

	// Handle input focus

	AdoptionFocusUpdate( _pNewParent );

	// Temporarily save old data

	WgWidget *	pOldParent 	= m_pParent;
	WgWidget *	pOldSibling = m_pNextSibling;
	WgRect		oldGeo		= m_geo;

	// Disconnect/Connect

	Disconnect();
	Connect( _pNewParent, _pBelow );


	// Update graphics, anchor/position, minTreeSize

	if( pOldParent )
	{
		pOldParent->RefreshTreeSizeLimit();

		if( m_bRenderedHere )
			pOldParent->RequestRender( oldGeo.x, oldGeo.y, oldGeo.w, oldGeo.h, pOldSibling, true );

	}

	m_bRenderedHere = false;
	UpdateGeometry(false);

	if( m_pParent != 0 )
	{
		RequestRender( 0, 0, m_geo.w, m_geo.h, 0, true );

		m_pParent->RefreshTreeSizeLimit();
	}

	return	true;
}

//____ IsChildOf() _________________________________________________________________

bool WgWidget::IsChildOf(WgWidget* pOther)
{
	for(WgWidget* parent = Parent(); parent; parent = parent->Parent())
	{
		if(parent == pOther)
			return true;
	}
	return false;
}

//____ GetNext() ______________________________________________________________

WgWidget * WgWidget::GetNext() const
{
	if( m_pFirstChild )
		return m_pFirstChild;				// We had a child, return it.

	if( m_pNextSibling )
		return m_pNextSibling;				// We had a sibling, return it.

	// Iterate through our ancestry and return first next sibling
	// of them that we can find.

	WgWidget * p = m_pParent;
	while( p != 0 )
	{
		if( p->m_pNextSibling )
			return p->m_pNextSibling;

		p = p->m_pParent;
	}

	return 0;
}

//____ GetPrev() ______________________________________________________________

WgWidget * WgWidget::GetPrev() const
{
	WgWidget * p = m_pPrevSibling;			// Check for previous sibling.

	if( p )
	{
		// We had a sibling, recurse through its last children.

		while( p->m_pLastChild )
			p = p->m_pLastChild;

		return p;							// Return sibling or last child/grandchild of it.
	}

	return m_pParent;
}

//____ GetLastOffspring() _____________________________________________________

WgWidget * WgWidget::GetLastOffspring() const
{
	WgWidget * p = m_pLastChild;

	if( !p )
		return 0;

	while( p->m_pLastChild )
		p = p->m_pLastChild;

	return p;
}


//____ Root() _________________________________________________________________
/**
	@return The root for the branch this widget is a part of. Not necessarily a
					WgRoot-widget.
*/

WgWidget * WgWidget::Root( void )
{
	if( m_pParent == 0 )
		return this;

	WgWidget * pTmp = m_pParent;
	while( pTmp->m_pParent != 0 )
		pTmp = pTmp->m_pParent;

	return pTmp;
}

//____ Enable() _______________________________________________________________
/**
	Enables a previously disabled widget. All widgets are enabled by default.
*/

void	WgWidget::Enable()
{
	if( !m_bEnabled )
	{
		m_bEnabled = true;
		DoMyOwnDisOrEnable();
	}
}


//____ Disable() ______________________________________________________________
/**
	Disables this widget, that is, redraws it with its specified disabled-look and
	set it to ignore user events.
*/

void	WgWidget::Disable()
{
	if( m_bEnabled )
	{
		m_bEnabled = false;
		DoMyOwnDisOrEnable();
	}
}

//____ HideBranch() ___________________________________________________________

void WgWidget::HideBranch()
{
	if( !m_bHidden )
	{
		m_bHidden = true;

		if( m_pParent )
			m_pParent->RequestRender( m_geo.x, m_geo.y, m_geo.w, m_geo.h, m_pNextSibling, true );
	}
}

//____ ShowBranch() ___________________________________________________________

void WgWidget::ShowBranch()
{
	if( m_bHidden )
	{
		m_bHidden = false;

		if( m_pParent )
			m_pParent->RequestRender( m_geo.x, m_geo.y, m_geo.w, m_geo.h, m_pNextSibling, true );
	}
}


//____ IsHidden() _____________________________________________________________

bool WgWidget::IsHidden()
{
	WgWidget * p = this;

	while( p )
	{
		if( p->m_bHidden )
			return true;
		p = p->m_pParent;
	}
	return false;
}


//____ EnableBranch() _________________________________________________________
/**
	Sets this widget and all of its subwidgets to enabled state.
*/

void	WgWidget::EnableBranch()
{
	Enable();
	for( WgWidget * pTmp = m_pFirstChild ; pTmp != 0 ; pTmp = pTmp->m_pNextSibling )
		pTmp->EnableBranch();
}


//____ DisableBranch() ________________________________________________________
/**
	Disables this widget and all of its subwidgets, that is, redraws them with
	their specified disabled-look and set them to ignore user events.
*/

void	WgWidget::DisableBranch()
{
	Disable();
	for( WgWidget * pTmp = m_pFirstChild ; pTmp != 0 ; pTmp = pTmp->m_pNextSibling )
		pTmp->DisableBranch();
}


//____ SetConstrainChildren() _________________________________________________

void WgWidget::SetConstrainChildren( bool bConstrain )
{
	if( m_bConstrainChildren != bConstrain )
	{
		m_bConstrainChildren = bConstrain;
		RefreshTreeSizeLimit();
	}
}


//____ SetPointerMask() _______________________________________________________

void	WgWidget::SetPointerMask( PointerMask mask )
{
	m_pointerMask = mask;
}


//____ SetOrigo(1) ___________________________________________________________
/**
	Sets origo for the widget, that is, which position along parents axies are
	considered to be cordinate (0,0).
*/

bool WgWidget::SetOrigo( WgOrigo _origo )
{
	m_origo1 = _origo;
	m_origo2 = _origo;
	UpdateGeometry( false );
	return	true;
}

//____ SetOrigo(2) ____________________________________________________________
/**
	Sets individual origo cordinates for the top-left and bottom-right corner of
	this widget. The bottom-right corner must have an origo position that is either
	equal to or further lower-left than the top-left corner.

	Giving the top-left and bottom-right corners different origos causes the
	widget to resize when its parent resizes.

	The hotspot portion of the origos are ignored since they are predetermined to
	be in the top-left and bottom-right respectively.

	@return False if specified bottom-right position is above or left of specified
					top-right.

	TODO: Maybe make widget "stay put" with its current geometry although origos
				are changed?

*/

bool WgWidget::SetOrigo( WgOrigo _topLeft, WgOrigo _bottomRight )
{
	if( _topLeft.anchorX() > _bottomRight.anchorX() || _topLeft.anchorY() > _bottomRight.anchorY() )
		return false;

	m_origo1 = WgOrigo::specific(_topLeft.anchorX(), _topLeft.anchorY(), 0, 0);
	m_origo2 = WgOrigo::specific(_bottomRight.anchorX(), _bottomRight.anchorY(), 0, 0);

	UpdateGeometry( false );
	return	true;
}

//____ Move() _________________________________________________________________

bool WgWidget::Move( int _x, int _y )
{
	m_x1 += _x;
	m_x2 += _x;
	m_y1 += _y;
	m_y2 += _y;

	return UpdateGeometry( false );
}

//____ MoveX() ________________________________________________________________

bool WgWidget::MoveX( int _x )
{
	m_x1 += _x;
	m_x2 += _x;
  return UpdateGeometry( false );
}

//____ MoveY() ________________________________________________________________

bool WgWidget::MoveY( int _y )
{
	m_y1 += _y;
	m_y2 += _y;

	return UpdateGeometry( false );
}

//____ SetPosX() ______________________________________________________________

bool WgWidget::SetPosX( int _x )
{
	m_x2 += _x - m_x1;
	m_x1 = _x;
  return UpdateGeometry( false );
}

//____ SetPosY() ______________________________________________________________

bool WgWidget::SetPosY( int _y )
{
	m_y2 += _y - m_y1;
	m_y1 = _y;
  return UpdateGeometry( false );
}


//____ SetPos() _______________________________________________________________
/**
		Sets the origo-related position of this widget.

		@return False if position couldn't be set as specified, in which case it's
						set as closely as possible.
*/

bool WgWidget::SetPos( int _x, int _y )
{
	m_x2 += _x - m_x1;
	m_x1 = _x;
	m_y2 += _y - m_y1;
	m_y1 = _y;
  return UpdateGeometry( false );
}

//____ MinSize() ______________________________________________________________
/**
	Sets the minimum size the widget may be resized to. The real minimum size
	of this widget might actually be larger due to constraints indirectly forced
	upon it by its children (a child that will stretch with parent due to geometry
	settings and run into its size constraints earlier will force its parent to
	not become any smaller) or by limits set by the widget-type itself.

	If either width or height is larger than earlier specified maximum size,
	the minimum size values are adjusted downwards.

	@return For the moment this method always returns true.
*/

bool WgWidget::MinSize( int _w, int _h )
{
	m_sizeLimit_user.minW = _w;
	m_sizeLimit_user.minH = _h;

	if( _w > m_sizeLimit_user.maxW )
		m_sizeLimit_user.maxW = _w;

	if( _h > m_sizeLimit_user.maxH )
		m_sizeLimit_user.maxH = _h;

	RefreshSizeLimit();
	UpdateGeometry(true);
	return	true;
}

//____ MaxSize() ______________________________________________________________
/**
	Sets the maximum size the widget may be resized to. The real maximum size
	of this widget might actually be smaller due to constraints indirectly forced
	upon it by its children (a child that will stretch with parent due to geometry
	settings and run into its size constraints earlier will force its parent to
	not become any larger) or by limits set by the widget-type itself.

	If either width or height is smaller than earlier specified minimum size,
	the maximum size values are adjusted upwards.

	@return For the moment this method always returns true.
*/

bool WgWidget::MaxSize( int _w, int _h )
{
	m_sizeLimit_user.maxW = _w;
	m_sizeLimit_user.maxH = _h;

	if( _w < m_sizeLimit_user.minW )
		m_sizeLimit_user.minW = _w;

	if( _h < m_sizeLimit_user.minH )
		m_sizeLimit_user.minH = _h;

	RefreshSizeLimit();
	UpdateGeometry(true);
	return	true;
}


//____ Resize() _______________________________________________________________

bool WgWidget::Resize( int _w, int _h, bool bAllowMoveChildren )
{
	m_x2 += _w;
	m_y2 += _h;
  return UpdateGeometry( true, bAllowMoveChildren );
}

//____ ResizeWidth() __________________________________________________________

bool WgWidget::ResizeWidth( int _w, bool bAllowMoveChildren )
{
	m_x2 += _w;
  return UpdateGeometry( true, bAllowMoveChildren );
}

//____ ResizeHeight() _________________________________________________________

bool WgWidget::ResizeHeight( int _h, bool bAllowMoveChildren )
{
	m_y2 += _h;
  return UpdateGeometry( true, bAllowMoveChildren );
}

//____ SetWidth() _____________________________________________________________

bool WgWidget::SetWidth( int _w, bool bAllowMoveChildren )
{
//	m_x2 = m_x1 + _w;
	m_x2 += _w - m_geo.w;
	return UpdateGeometry( true, bAllowMoveChildren );
}

//____ SetHeight() ____________________________________________________________

bool WgWidget::SetHeight( int _h, bool bAllowMoveChildren  )
{
//	m_y2 = m_y1 + _h;
	m_y2 += _h - m_geo.h;
	return UpdateGeometry( true, bAllowMoveChildren );
}

//____ SetSize() ______________________________________________________________

bool WgWidget::SetSize( int _w, int _h, bool bAllowMoveChildren )
{
	m_x2 += _w - m_geo.w;
	m_y2 += _h - m_geo.h;

	//	m_x2 = m_x1 + _w;
	//	m_y2 = m_y1 + _h;
	return UpdateGeometry( true, bAllowMoveChildren );
}

//____ Modal() ________________________________________________________________
/**
	Sets the widget to being modal, forcing it to stay on top of all non-modal
	siblings and block user input to all siblings. This way you can for example
	force the user to acknowledge a certain dialog box and fill it in before being
	able to touch any other widgets.

	Also moves the widget ontop of all other widgets.
*/

void WgWidget::Modal( void )
{

	m_bModal = true;

	if( m_pParent )
	{
		m_pParent->RequestRender( m_geo.x, m_geo.y, m_geo.w, m_geo.h, m_pNextSibling, true ); // Necessary? Always?

		Disconnect();
		Connect( m_pParent, 0 );

		m_bRenderedHere = false;
		RequestRender( 0, 0, m_geo.w, m_geo.h, 0, false );
	}
}

//____ UnModal() ______________________________________________________________
/**
	Turns a previously modal widget back to non-modal state. Moves the widget to
	the top of all non-modal widgets.
*/

void WgWidget::UnModal( void )
{
	WgWidget	* pTmp;

	m_bModal = false;

	if( m_pParent )
	{
		m_pParent->RequestRender( m_geo.x, m_geo.y, m_geo.w, m_geo.h, m_pNextSibling, true );	// Necessary? Always?

		Disconnect();

		pTmp = m_pParent->m_pFirstChild;
		while( pTmp != 0 && !pTmp->m_bModal )
			pTmp = pTmp->m_pNextSibling;

		Connect( m_pParent, pTmp );

		m_bRenderedHere = false;
		RequestRender( 0, 0, m_geo.w, m_geo.h, 0, false );
	}
}


//____ Top() __________________________________________________________________
/**
	Moves the widget to the top of the window-stack of its siblings. Non-modal
	widgets can not be moved ontop of modal widgets, but if there are two or more
	modal widgets, they can be rearranged in the same way.

	@return True if the widget managed to reach the top, false if it already was
					at the top, doesn't have any siblings or is non-modal and there are
					modal widgets on the stack. In the later case the widget is still
					moved as far up as possible.
*/

bool WgWidget::Top( void )
{
	if( m_pParent == 0 )
		return	false;

	m_pParent->RequestRender( m_geo.x, m_geo.y, m_geo.w, m_geo.h, m_pNextSibling, true );

	Disconnect();

	if( m_bModal )
		Connect( m_pParent, 0 );		// Put on top of all
	else
	{
		WgWidget	* pTmp = m_pParent->m_pFirstChild;
		while( pTmp != 0 && !pTmp->m_bModal )
			pTmp = pTmp->m_pNextSibling;

		Connect( m_pParent, pTmp );
		if( pTmp != 0 )
			return	false;				// Couldn't reach the top, modal in the way.
	}

	m_bRenderedHere = false;
	RequestRender( 0, 0, m_geo.w, m_geo.h, 0, false );
	return	true;
}


//____ Bottom() _______________________________________________________________
/**
	Moves the widget to the bottom in the window-stack of its siblings. Modal
	widgets can not be moved below non-modal widgets, but if there are two or more
	modal widgets, they can be rearranged in the same way.

	@return True if the widget managed to reach the bottom, false if it already
					was at the bottom, doesn't have any siblings or is modal and there
					are non-modal widgets in the stack. In the later case the widget is
					still moved as far down as possible.
*/

bool WgWidget::Bottom( void )
{
	if( m_pParent == 0 )
		return	false;

	m_pParent->RequestRender( m_geo.x, m_geo.y, m_geo.w, m_geo.h, m_pNextSibling, true );

	Disconnect();

	if( !m_bModal )
		Connect( m_pParent, m_pParent->m_pFirstChild );
	else
	{
		WgWidget	* pTmp = m_pParent->m_pFirstChild;
		while( pTmp != 0 && !pTmp->m_bModal )
			pTmp = pTmp->m_pNextSibling;
		Connect( m_pParent, pTmp );
		if( m_pParent->m_pFirstChild != this )
			return	false;			// Couldn't reach bottom, non-modal below.
	}
	m_bRenderedHere = false;
	RequestRender( 0, 0, m_geo.w, m_geo.h, 0, false );
	return	true;
}

//____ Up() ___________________________________________________________________
/**
	Moves the widget one step up in the window-stack of its siblings. Non-modal
	widgets can not be moved on top of modal widgets, but if there are two or more
	modal widgets, they can be rearranged in the same way.

	@return True if the widget was moved up, false if it already was at the top or
					doesn't have any siblings or only modal widgets are on top of it.
*/

bool WgWidget::Up( void )
{
	if( m_pParent == 0 )
		return	false;

	if( m_pNextSibling == 0 || (!m_bModal && m_pNextSibling->m_bModal) )
		return	false;

	m_pParent->RequestRender( m_geo.x, m_geo.y, m_geo.w, m_geo.h, m_pNextSibling, true );

	Disconnect();
	Connect( m_pParent, m_pNextSibling->m_pNextSibling );

	m_bRenderedHere = false;
	RequestRender( 0, 0, m_geo.w, m_geo.h, 0, false );
	return	true;
}

//____ Down() _________________________________________________________________
/**
	Moves the widget one step down in the window-stack of its siblings. Modal
	widgets can not be moved down under non-modal widgets, but if there are two
	or more modal widgets, they can be arranged inbetween themselves.

	@return True if the widget was moved down, false if it already was at the
					bottom or doesn't have any siblings or if this is a modal widget and
					there only are non-modal widgets below it.
*/

bool WgWidget::Down( void )
{
	if( m_pParent == 0 )
		return	false;

	WgWidget * pPrev = m_pPrevSibling;
	if( pPrev == 0 )
		return false;					// Already at bottom

	if( m_bModal && !pPrev->m_bModal )
		return	false;					// Already lowest among modals.

	m_pParent->RequestRender( m_geo.x, m_geo.y, m_geo.w, m_geo.h, m_pNextSibling, true );

	Disconnect();
	Connect( m_pParent, pPrev );

	m_bRenderedHere = false;
	RequestRender( 0, 0, m_geo.w, m_geo.h, 0, false );
	return	true;
}

//____ MoveAbove() ____________________________________________________________

bool WgWidget::MoveAbove( WgWidget * _pSibling )
{
	if( m_pParent == 0 || _pSibling == 0 || _pSibling->m_pParent != m_pParent )
		return	false;

	// Get sibling to put ourself below.

	WgWidget * pAbove = _pSibling->m_pNextSibling;

	// Break if it already is us.

	if( pAbove == this )
		return true;

	// We need to have the same modality as either the sibling below or above us,
	// unless we are modal and wants to be put on top.

	if( (m_bModal != _pSibling->m_bModal) )
	{
		if(pAbove && m_bModal != pAbove->m_bModal)
			return false;

		if(!pAbove && m_bModal == false )
			return false;

	}

	//

	m_pParent->RequestRender( m_geo.x, m_geo.y, m_geo.w, m_geo.h, m_pNextSibling, true );

	Disconnect();
	Connect( m_pParent, pAbove );

	m_bRenderedHere = false;
	RequestRender( 0, 0, m_geo.w, m_geo.h, 0, false );
	return	true;
}

//____ MoveBelow() ____________________________________________________________

bool WgWidget::MoveBelow( WgWidget * _pSibling )
{
	if( m_pParent == 0 || _pSibling == 0 || _pSibling->m_pParent != m_pParent )
		return	false;

	// Find the sibling before _pSibling (if any).

	WgWidget * pBefore = _pSibling->m_pPrevSibling;

	if( pBefore == 0 )
		return Bottom();						// No sibling before _pSibling, so we just send ourselves to the bottom.

	// Break if it already is us.

	if( pBefore == this )
		return true;

	// We need to have the same modality as either the sibling below or above us,
	// unless we are unmodal and wants to be put lowest.

	if( (m_bModal != _pSibling->m_bModal) )
	{
		if(pBefore && m_bModal != pBefore->m_bModal)
			return false;

		if(!pBefore && m_bModal == true )
			return false;

	}

	//

	m_pParent->RequestRender( m_geo.x, m_geo.y, m_geo.w, m_geo.h, m_pNextSibling, true );

	Disconnect();
	Connect( m_pParent, _pSibling );

	m_bRenderedHere = false;
	RequestRender( 0, 0, m_geo.w, m_geo.h, 0, false );
	return	true;
}




//____ Intercept() ____________________________________________________________

bool  WgWidget::Intercept( ActionSource _actionGroup, InterceptionRules _rules )
{

  switch( _rules )
  {
    case  PASS:
      m_actionFilterBlock &= ~_actionGroup;
      m_actionFilterCopy &= ~_actionGroup;
      break;
    case  BLOCK:
      m_actionFilterBlock |= _actionGroup;
      m_actionFilterCopy &= ~_actionGroup;
      break;
    case  INTERCEPT_PASS:
      m_actionFilterBlock &= ~_actionGroup;
      m_actionFilterCopy |= _actionGroup;
      break;
    case  INTERCEPT_BLOCK:
      m_actionFilterBlock |= _actionGroup;
      m_actionFilterCopy |= _actionGroup;
      break;
    default:
      return  false;
  }
  return  true;
}


//____ GetInputObj() _______________________________________________________

WgInput* WgWidget::GetInputObj()
{
	WgInput * pInputObj = 0;

	if( IsRootWidget() )
	{
		pInputObj = ((Wdg_Root*)this)->InputObj();
	}
	else
	{
		WgWidget * pRoot = Root();

		if( pRoot && pRoot->IsRootWidget() )
			pInputObj = ((Wdg_Root*)pRoot)->InputObj();
	}

	return pInputObj;
}


//____ GrabInputFocus() _______________________________________________________

bool WgWidget::GrabInputFocus( void )
{
	WgInput * pInputObj = GetInputObj();

	if( !pInputObj )
		return false;

	if( pInputObj->getFocusedWidget() != this )
		pInputObj->setFocusedWidget( this );
	return true;
}


//____ RemoveInputFocus() _______________________________________________________

bool WgWidget::RemoveInputFocus()
{
	WgInput * pInputObj = GetInputObj();

	if( !pInputObj )
		return false;

	if( pInputObj->getFocusedWidget() == this )
		pInputObj->setFocusedWidget( 0 );
	return true;
}



//____ Abs2local() ____________________________________________________________
/**
	Converts screen cordinates to cordinates relative top-left corner of this widget.
*/

void WgWidget::Abs2local( int * _pX, int * _pY ) const
{
	const WgWidget	* pTmp = this;

	while( pTmp != 0 )
	{
		*_pX -= pTmp->m_geo.x;
		*_pY -= pTmp->m_geo.y;
		pTmp = pTmp->m_pParent;
	}

}

//____ ScreenGeometry() _______________________________________________________
/**
		@return		The geometry of this widget expressed in real screen cordinates.
*/

WgRect WgWidget::ScreenGeometry() const
{
	WgRect geo = m_geo;

	for( WgWidget * p = m_pParent ; p != 0 ; p = p->m_pParent )
	{
		geo.x += p->m_geo.x;
		geo.y += p->m_geo.y;
	}

	return geo;
}



//=============================================================================
//                      >>> PRIVATE FUNCTIONS <<<
//=============================================================================

//____ GotInputFocus() ________________________________________________________

void WgWidget::GotInputFocus( void )
{
	DoMyOwnInputFocusChange( true );
}


//____ LostInputFocus() _______________________________________________________

void WgWidget::LostInputFocus( void )
{
	DoMyOwnInputFocusChange( false );
}

//____ ActionRespond() ________________________________________________________

void WgWidget::ActionRespond( WgInput::UserAction _action, int _button, const WgActionDetails& _info, const WgInput& _inputObj )
{
	if( !m_bEnabled )
		return;

	switch( _action )
	{
		case WgInput::POINTER_ENTER:
			Emit( PointerEnter() );
			break;

		case WgInput::POINTER_OVER:
		{
			Emit( PointerOver() );
			int x = _info.x;
			int y = _info.y;
			Emit( PointerOverPos(), x, y );
			Emit( PointerOverPosX(), x );
			Emit( PointerOverPosY(), y );
			Abs2local( &x, &y );
			Emit( PointerOverOfs(), x, y );
			Emit( PointerOverOfsX(), x );
			Emit( PointerOverOfsY(), y );
			break;
		}

		case WgInput::POINTER_OUTSIDE_MODAL:
		{
			Emit( PointerOutsideModal() );
			int x = _info.x;
			int y = _info.y;
			Emit( PointerOutsideModalPos(), x, y );
			Emit( PointerOutsideModalPosX(), x );
			Emit( PointerOutsideModalPosY(), y );
			Abs2local( &x, &y );
			Emit( PointerOutsideModalOfs(), x, y );
			Emit( PointerOutsideModalOfsX(), x );
			Emit( PointerOutsideModalOfsY(), y );
			break;
		}


		case WgInput::POINTER_EXIT:
			Emit( PointerExit() );
			break;

		case WgInput::BUTTON_PRESS:
		{
			Emit( ButtonPress(_button) );
			int x = _info.x;
			int y = _info.y;
     		Emit( ButtonPressPos(_button), x, y );
     		Emit( ButtonPressPosX(_button), x );
     		Emit( ButtonPressPosY(_button), y );
			Abs2local( &x, &y );
			Emit( ButtonPressOfs(_button), x, y );
			Emit( ButtonPressOfsX(_button), x );
			Emit( ButtonPressOfsY(_button), y );
			break;
		}

		case WgInput::BUTTON_DOWN:
		{
			Emit( ButtonDown(_button) );
			int x = _info.x;
			int y = _info.y;
			Emit( ButtonDownPos(_button), x, y );
			Emit( ButtonDownPosX(_button), x );
			Emit( ButtonDownPosY(_button), y );
			Abs2local( &x, &y );
			Emit( ButtonDownOfs(_button), x, y );
			Emit( ButtonDownOfsX(_button), x );
			Emit( ButtonDownOfsY(_button), y );

			const WgActionDetails * pDetails = _inputObj.getPressDetails(_button);
			Emit( ButtonDragTotal(_button), _info.x - pDetails->x, _info.y - pDetails->y );
			Emit( ButtonDragTotalX(_button), _info.x - pDetails->x );
			Emit( ButtonDragTotalY(_button), _info.y - pDetails->y );

			pDetails = _inputObj.getMouseOverDetails();
			Emit( ButtonDragSinceLast(_button), _info.x - pDetails->x, _info.y - pDetails->y );
			Emit( ButtonDragSinceLastX(_button), _info.x - pDetails->x );
			Emit( ButtonDragSinceLastY(_button), _info.y - pDetails->y );
			break;
		}

		case WgInput::BUTTON_REPEAT:
		{
			Emit( ButtonRepeat(_button) );
			int x = _info.x;
			int y = _info.y;
			Emit( ButtonRepeatPos(_button), x, y );
			Emit( ButtonRepeatPosX(_button), x );
			Emit( ButtonRepeatPosY(_button), y );
			Abs2local( &x, &y );
			Emit( ButtonRepeatOfs(_button), x, y );
			Emit( ButtonRepeatOfsX(_button), x );
			Emit( ButtonRepeatOfsY(_button), y );
			break;
        }

		case WgInput::BUTTON_RELEASE:
		{
     		Emit( ButtonRelease(_button) );
			int x = _info.x;
			int y = _info.y;
			Emit( ButtonReleasePos(_button), x, y );
			Emit( ButtonReleasePosX(_button), x );
			Emit( ButtonReleasePosY(_button), y );
			Abs2local( &x, &y );
			Emit( ButtonReleaseOfs(_button), x, y );
			Emit( ButtonReleaseOfsX(_button), x );
			Emit( ButtonReleaseOfsY(_button), y );
			break;
		}

		case WgInput::BUTTON_RELEASE_OUTSIDE:
		{
     		Emit( ButtonReleaseOutside(_button) );
			int x = _info.x;
			int y = _info.y;
			Emit( ButtonReleaseOutsidePos(_button), x, y );
			Emit( ButtonReleaseOutsidePosX(_button), x );
			Emit( ButtonReleaseOutsidePosY(_button), y );
			Abs2local( &x, &y );
			Emit( ButtonReleaseOutsideOfs(_button), x, y );
			Emit( ButtonReleaseOutsideOfsX(_button), x );
			Emit( ButtonReleaseOutsideOfsY(_button), y );
			break;
		}


		case WgInput::BUTTON_CLICK:
		{
     		Emit( ButtonClick(_button) );
			int x = _info.x;
			int y = _info.y;
			Emit( ButtonClickPos(_button), x, y );
			Emit( ButtonClickPosX(_button), x );
			Emit( ButtonClickPosY(_button), y );
			Abs2local( &x, &y );
			Emit( ButtonClickOfs(_button), x, y );
			Emit( ButtonClickOfsX(_button), x );
			Emit( ButtonClickOfsY(_button), y );
			break;
		}

		case WgInput::BUTTON_DOUBLECLICK:
		{
     		Emit( ButtonDoubleClick(_button) );
			int x = _info.x;
			int y = _info.y;
			Emit( ButtonDoubleClickPos(_button), x, y );
			Emit( ButtonDoubleClickPosX(_button), x );
			Emit( ButtonDoubleClickPosY(_button), y );
			Abs2local( &x, &y );
			Emit( ButtonDoubleClickOfs(_button), x, y );
			Emit( ButtonDoubleClickOfsX(_button), x );
			Emit( ButtonDoubleClickOfsY(_button), y );
			break;
		}

		case WgInput::WHEEL_ROLL:
		{
			Emit( WheelRoll( _button), _info.rolldistance );
			break;
		}

		case WgInput::BUTTON_PRESS_OUTSIDE_MODAL:
		{
			Emit( ButtonPressOutsideModal(_button) );
			break;
		}

		case WgInput::BUTTON_RELEASE_OUTSIDE_MODAL:
		{
			Emit( ButtonReleaseOutsideModal(_button) );
			break;
		}

		case WgInput::WHEEL_ROLL_OUTSIDE_MODAL:
		{
			Emit( WheelRollOutsideModal( _button), _info.rolldistance );
			break;
		}

		case WgInput::KEY_PRESS:
		{
			Emit( KeyPress(_button, _info.modifier) );
//			Emit( sig_keyPressAnyModifier[_button] );
			break;
		}

		case WgInput::KEY_DOWN:
		{
			Emit( KeyDown(_button, _info.modifier) );
//			Emit( sig_keyDownAnyModifier[_button] );
			break;
		}

		case WgInput::KEY_REPEAT:
		{
			Emit( KeyRepeat(_button, _info.modifier) );
//			Emit( sig_keyRepeatAnyModifier[_button] );
			break;
		}

		case WgInput::KEY_RELEASE:
		{
			Emit( KeyRelease(_button, _info.modifier) );
//			Emit( sig_keyReleaseAnyModifier[_button] );
			break;
		}

		case WgInput::CHARACTER:
            break;
	}

	DoMyOwnActionRespond( _action, _button, _info, _inputObj );
}


//____ Local2abs() ____________________________________________________________
/**
	Converts cordinates from widgets local cordinate system (top-left corner is
	0,0) to screen cordinates.
*/

void WgWidget::Local2abs( int * _pX, int * _pY ) const
{
	const WgWidget	* pTmp = this;

	while( pTmp != 0 )
	{
		*_pX += pTmp->m_geo.x;
		*_pY += pTmp->m_geo.y;
		pTmp = pTmp->m_pParent;
	}

}


//____ UpdateX() ______________________________________________________________

void WgWidget::UpdateX( const WgUpdateInfo& _info )
{

  // Call the widget-types own update

 	DoMyOwnUpdate( _info );

	// Call children recursively

	for( WgWidget * pTmp = m_pFirstChild ; pTmp != 0 ; pTmp = pTmp->m_pNextSibling )
		pTmp->UpdateX( _info );

}

//____ UpdateGeometry() _______________________________________________________

bool WgWidget::UpdateGeometry( bool bPreferResize, bool bMoveChildren )
{

	//

	WgRect		geo;
	bool		retVal = true;

	if( m_pParent )
	{

/*
		float xf = m_origo1.anchorX() * m_pParent->m_geo.w;
		float yf = m_origo1.anchorY() * m_pParent->m_geo.h;

		xf -= m_origo1.hotspotX() * (m_x2 - m_x1);
		yf -= m_origo1.hotspotY() * (m_y2 - m_y1);

		geo.x = m_x1 + (int) xf;
		geo.y = m_y1 + (int) yf;

		xf = m_origo2.anchorX() * m_pParent->m_geo.w;
		yf = m_origo2.anchorY() * m_pParent->m_geo.h;

		xf -= m_origo2.hotspotX() * (m_x2 - m_x1);
		yf -= m_origo2.hotspotY() * (m_y2 - m_y1);

		geo.w = (m_x2 + (int) xf) - geo.x;
		geo.h = (m_y2 + (int) yf) - geo.y;

*/
		geo.x = m_x1 + (int) m_origo1.calcOfsX( m_pParent->m_geo.w, (m_x2 - m_x1) );
		geo.y = m_y1 + (int) m_origo1.calcOfsY( m_pParent->m_geo.h, (m_y2 - m_y1) );

		geo.w = (m_x2 + (int) m_origo2.calcOfsX( m_pParent->m_geo.w, (m_x2 - m_x1) )) - geo.x;
		geo.h = (m_y2 + (int) m_origo2.calcOfsY( m_pParent->m_geo.h, (m_y2 - m_y1) )) - geo.y;

	}
	else
	{
		geo.x = 0;
		geo.y = 0;

		if( m_origo1.anchorX() == m_origo2.anchorX() && m_origo2.hotspotX() == m_origo2.hotspotX() )
			geo.w = m_x2 - m_x1;
		else
			geo.w = m_sizeLimit.maxW;

		if( m_origo1.anchorY() == m_origo2.anchorY() && m_origo2.hotspotY() == m_origo2.hotspotY() )
			geo.h = m_y2 - m_y1;
		else
			geo.h = m_sizeLimit.maxH;
	}


	// Limit values & error checks

	// First limit against our own limit values and children.

	WgMinMax2D sizeLimit;
	if( bMoveChildren )
		sizeLimit.Union( m_sizeLimit_self, m_sizeLimit_user );
	else
		sizeLimit = m_sizeLimit;


	if( geo.w < sizeLimit.minW )
	{
		m_x2 += sizeLimit.minW - geo.w;
		geo.w = sizeLimit.minW;
		retVal = false;
	}
	else if( geo.w > sizeLimit.maxW )
	{
		m_x2 -= geo.w - sizeLimit.maxW;
		geo.w = sizeLimit.maxW;
		retVal = false;
	}

	if( geo.h < sizeLimit.minH )
	{
		m_y2 += sizeLimit.minH - geo.h;
		geo.h = sizeLimit.minH;
		retVal = false;
	}
	else if( geo.h > sizeLimit.maxH )
	{
		m_y2 -= geo.h - sizeLimit.maxH;
		geo.h = sizeLimit.maxH;
		retVal = false;
	}


	if( m_pParent && m_pParent->m_bConstrainChildren )
	{
		// Then limit against our ancestry

		if( geo.x < 0 )
		{
			m_x1 -= geo.x;
			m_x2 -= geo.x;
			geo.x = 0;
			retVal = false;
		}

		if( geo.y < 0 )
		{
			m_y1 -= geo.y;
			m_y2 -= geo.y;
			geo.y = 0;
			retVal = false;
		}


		if( geo.x + geo.w > m_pParent->m_geo.w )
		{
			int over = geo.x + geo.w - m_pParent->m_geo.w;
			int	sizeDec = 0;
			int	posDec = 0;

			if( bPreferResize )
			{
				sizeDec = over;
				if( geo.w - sizeDec < m_sizeLimit.minW )
					posDec = m_sizeLimit.minW - (geo.w - sizeDec );

				sizeDec -= posDec;

				if( posDec > geo.x )
					posDec = geo.x;
			}
			else
			{
				posDec = over;
				if( posDec > geo.x )
				{
					sizeDec = posDec - geo.x;

					if( geo.w - sizeDec < m_sizeLimit.minW )
						sizeDec = geo.w - m_sizeLimit.minW;

					posDec = geo.x;
				}
			}
			geo.x -= posDec;
			geo.w -= sizeDec;

			m_x1 -= posDec;
			m_x2 -= posDec + sizeDec;
			retVal = false;
		}

		if( geo.y + geo.h > m_pParent->m_geo.h )
		{
			int over = geo.y + geo.h - m_pParent->m_geo.h;
			int	sizeDec = 0;
			int	posDec = 0;

			if( bPreferResize )
			{
				sizeDec = over;
				if( geo.h - sizeDec < m_sizeLimit.minH )
					posDec = m_sizeLimit.minH - (geo.h - sizeDec );

				sizeDec -= posDec;

				if( posDec > geo.y )
					posDec = geo.y;
			}
			else
			{
				posDec = over;
				if( posDec > geo.y )
				{
					sizeDec = posDec - geo.y;

					if( geo.h - sizeDec < m_sizeLimit.minH )
						sizeDec = geo.h - m_sizeLimit.minH;

					posDec = geo.y;
				}
			}
			geo.y -= posDec;
			geo.h -= sizeDec;

			m_y1 -= posDec;
			m_y2 -= posDec + sizeDec;
			retVal = false;
		}
	}

	//


	if( m_geo != geo )
	{
		// Emit signals

		//

		if( m_pParent != 0 && m_bRenderedHere )
			m_pParent->RequestRender( m_geo.x, m_geo.y, m_geo.w, m_geo.h, m_pNextSibling, true );

		bool bSizeChanged = false;
		if( geo.w != m_geo.w || geo.h != m_geo.h )
			bSizeChanged = true;

		WgRect oldGeo = m_geo;
		m_geo = geo;


		m_bRenderedHere = false;
		RequestRender( 0, 0, m_geo.w, m_geo.h, 0, true );
		if( m_pParent != 0 )
			m_pParent->RefreshTreeSizeLimit();

		// Update children if size has changed.

		if( bSizeChanged )
		{
			DoMyOwnGeometryChange( oldGeo, m_geo );

			for( WgWidget * pCh = m_pFirstChild ; pCh != 0 ; pCh = pCh->m_pNextSibling )
			{
				pCh->UpdateGeometry(false, bMoveChildren);
			}
		}

			Emit( WgSignal::GeometryChange() );
	}

	return retVal;
}


//____ AdoptionFocusUpdate() __________________________________________________

void WgWidget::AdoptionFocusUpdate( WgWidget * _pNewParent )
{
	if( !m_pParent )
		return;																		// We had no parent.

	WgWidget * pOldRoot = Root();
	if( !pOldRoot->IsRootWidget() )
		return;																		// Branch root was not a root object, consequently neither we nor children can have focus.

	// Get root for our new branch

	WgWidget * pNewRoot = _pNewParent;

	if( _pNewParent != 0 )
	{
		WgWidget * pParentRoot = _pNewParent->Root();
		if( pParentRoot != 0 )
			pNewRoot = pParentRoot;
	}

	// Don't change focus if root remains the same.

	if( pNewRoot == pOldRoot )
		return;


	// Find out if we or any of our children has focus, in which case we give it to our old parent.

	WgInput * pInputObj = ((Wdg_Root*)pOldRoot)->InputObj();

	for( WgWidget * pWdg = pInputObj->getFocusedWidget() ; pWdg != 0 ; pWdg = pWdg->m_pParent )
		if( pWdg == this )
		{
			m_pParent->GrabInputFocus();
			break;
		}

	// Make sure that we and our children won't get any PointerExit or similar after having removed widget.

	removeFromInputRecursively( ((Wdg_Root*)pOldRoot)->InputObj() );


}

//____ removeFromInputRecursively() ___________________________________________

void WgWidget::removeFromInputRecursively( WgInput * pInput )
{
	pInput->widgetRemoved(this);

	WgWidget * pChild = m_pFirstChild;
	while( pChild )
	{
		pChild->removeFromInputRecursively( pInput );
		pChild = pChild->NextSibling();
	}
}



//____ Disconnect() ___________________________________________________________

// Does NOT clear the objects own pointers to parents and siblings!

bool WgWidget::Disconnect( void )
{
	if( m_pParent != 0 )
	{
		if( m_pPrevSibling )
			m_pPrevSibling->m_pNextSibling = m_pNextSibling;
		else
			m_pParent->m_pFirstChild = m_pNextSibling;

		if( m_pNextSibling )
			m_pNextSibling->m_pPrevSibling = m_pPrevSibling;
		else
			m_pParent->m_pLastChild = m_pPrevSibling;
	}
	return	true;
}

//____ Connect() ______________________________________________________________

bool WgWidget::Connect( WgWidget * _pNewParent, WgWidget * _pPutBefore )
{
	if( _pNewParent == 0 )
	{
		m_pParent = 0;
		m_pPrevSibling = 0;
		m_pNextSibling = 0;
		return	true;
	}

	// Update our own links

	if( _pPutBefore )
	{
		m_pPrevSibling = _pPutBefore->m_pPrevSibling;
		m_pNextSibling = _pPutBefore;
	}
	else
	{
		m_pPrevSibling = _pNewParent->m_pLastChild;
		m_pNextSibling = 0;
	}		

	// Update surrounding links

	if( m_pPrevSibling )
		m_pPrevSibling->m_pNextSibling = this;
	else
		_pNewParent->m_pFirstChild = this;

	if( m_pNextSibling )
		m_pNextSibling->m_pPrevSibling = this;
	else
		_pNewParent->m_pLastChild = this;

	//

	m_pParent = _pNewParent;
	return	true;
}


//____ FindOccupant() _________________________________________________________

WgWidget * WgWidget::FindOccupant( int _x, int _y, bool _bCheckModal, WgWidget * _bestSoFar, WgWidget ** _wpBlockingModal )
{
	_x -= m_geo.x; _y -= m_geo.y;

	if( _bCheckModal && m_bModal )
	{
		if( _wpBlockingModal )
			* _wpBlockingModal = this;

		_bestSoFar = 0;												// Ignore parent and earlier sibling-branches.
	}

	if( _x < 0 || _y < 0 || _x >= m_geo.w || _y >= m_geo.h )
	{
		// If we are outside our root we still need to let
		// modals directly under root say that they are blocking.
		// (this is a policy change and might give unwanted side effects
		//	with older code).

		if( this->IsRootWidget() && _wpBlockingModal )
		{
			for( WgWidget * pChi = m_pFirstChild ; pChi != 0 ; pChi = pChi->m_pNextSibling )
				_bestSoFar = pChi->FindOccupant( _x, _y, _bCheckModal, _bestSoFar, _wpBlockingModal );
		}

		return _bestSoFar;
	}

	if( m_pointerMask == POINTER_SOFT_OPAQUE || m_pointerMask == POINTER_OPAQUE || (m_pointerMask == POINTER_SOURCE_ALPHA && DoMyOwnMarkTest( _x, _y )) )
	{
		if( _wpBlockingModal )
			* _wpBlockingModal = 0;
		_bestSoFar = this;
	}

/*													// Why did I have this unworking alternative in use instead?
	if( !m_bOpaqueForMouse )						// Remember I changed earlier, but why...
		if( !DoMyOwnMarkTest( _x, _y ) )
			return _bestSoFar;

	_bestSoFar = this;
*/
	for( WgWidget * pChi = m_pFirstChild ; pChi != 0 ; pChi = pChi->m_pNextSibling )
	{
		if( !pChi->m_bHidden )
			_bestSoFar = pChi->FindOccupant( _x, _y, _bCheckModal, _bestSoFar, _wpBlockingModal );
	}

	return	_bestSoFar;
}


//____ RefreshTreeSizeLimit() _________________________________________________

void WgWidget::RefreshTreeSizeLimit( void )
{
	float			minW = 0;
	float			minH = 0;
	float			maxW = 10000;
	float			maxH = 10000;

	// If children are free floating we don't have any tree size limit...

	if( !m_bConstrainChildren )
	{
		if( m_sizeLimit_tree.minW != minW || m_sizeLimit_tree.minH != minH ||
			m_sizeLimit_tree.maxW != maxW || m_sizeLimit_tree.maxH != maxH )
		{
			m_sizeLimit_tree.minW = (Uint16) minW;
			m_sizeLimit_tree.minH = (Uint16) minH;
			m_sizeLimit_tree.maxW = (Uint16) maxW;
			m_sizeLimit_tree.maxH = (Uint16) maxH;
			RefreshSizeLimit();
		}
		return;
	}


	// Go through children and find out our minimum w/h

	WgWidget	* pChi = m_pFirstChild;
	while( pChi != 0 )
	{
		float		w, h;

		if( pChi->m_origo1 != pChi->m_origo2 )
		{
			// Min Width

      float   w1 = 0, w2 = 0, w3 = 0;

      if( pChi->m_origo1.anchorX() != pChi->m_origo2.anchorX() )              // Do we have a stretch area?
      {
        int   scalepix = pChi->m_sizeLimit.minW - (pChi->m_x2 - pChi->m_x1);   // Min amount of pixels in scaled part.
        w1 = scalepix / (pChi->m_origo2.anchorX() - pChi->m_origo1.anchorX());      // Min widget m_width to provide that.
      }

      if( pChi->m_x1 < 0 )                           // Does the widget reach left of its anchors?
        w2 = (-pChi->m_x2) / pChi->m_origo1.anchorX();   // Min widget m_width to provide the pixels we need left of anchor.

      if( pChi->m_x2 > 0 )                           // Does the widget reach right of its anchors?
        w3 = pChi->m_x2 / (1.f - pChi->m_origo2.anchorX());  // Min widget m_width to provide the pixels we need right of anchor.
		// hey, the line above can cause division by zero = NAN

      // Take the highest value as minimum m_width.

      w = w1;
      if( w2 > w )
        w = w2;
      if( w3 > w )
        w = w3;

      // Min Height

      float		h1 = 0, h2 = 0, h3 = 0;

      if( pChi->m_origo1.anchorY() != pChi->m_origo2.anchorY() )              // Do we have a stretch area?
      {
        int   scalepix = pChi->m_sizeLimit.minH - (pChi->m_y2 - pChi->m_y1);   // Min amount of pixels in scaled part.
        h1 = scalepix / (pChi->m_origo2.anchorY() - pChi->m_origo1.anchorY());      				// Min widget height to provide that.
      }

      if( pChi->m_y1 < 0 )                                   // Does the widget reach above its anchors?
        h2 = (-pChi->m_y1) / pChi->m_origo1.anchorY();  // Min widget height to provide the pixels we need above the anchor.

      if( pChi->m_y2 > 0 )                                       // Does the widget reach below its anchors?
        h3 = pChi->m_y2 / (1.f - pChi->m_origo2.anchorY()); // Min widget height to provide the pixels we need below the anchor.

      // Take the highest value as minimum m_width.

      h = h1;
      if( h2 > h )
        h = h2;
      if( h3 > h )
        h = h3;

      // If widget has a stretch area, it might affect max values as well!

      if( pChi->m_origo1.anchorX() != pChi->m_origo2.anchorX() )          // Check max m_width.
      {
        int   scalepix = pChi->m_sizeLimit.maxW - (pChi->m_x2 - pChi->m_x1); // Max amount of pixels in scaled part.
        float mW = scalepix / (pChi->m_origo2.anchorX() - pChi->m_origo1.anchorX());        // Widget m_width to provide that.

        if( mW < maxW )
          maxW = mW;
      }

      if( pChi->m_origo1.anchorY() != pChi->m_origo2.anchorY() )         // Check max height.
      {
        int   scalepix = pChi->m_sizeLimit.maxH - (pChi->m_y2 - pChi->m_y1); // Max amount of pixels in scaled part.
        float mH = scalepix / (pChi->m_origo2.anchorY() - pChi->m_origo1.anchorY());        // Widget m_width to provide that.

        if( mH < maxH )
          maxH = mH;
      }


		}
		else
		{
			// Width
																// Can we increase precision with float calculations here?
			w = (float) pChi->m_geo.w;
			h = (float) pChi->m_geo.h;

			float w1 = w * pChi->m_origo1.hotspotX();
			float w2 = w - w1;

			w1 -= pChi->m_x1;
			w2 += pChi->m_x1;

			if( w1 < 0.f )
				w1 = 0.f;
			if( w2 < 0.f )
				w2 = 0.f;

			if( pChi->m_origo1.anchorX() != 0.f )
				w1 /=  pChi->m_origo1.anchorX();

			if( pChi->m_origo1.anchorX() != 1.f )
				w2 /= 1.f - pChi->m_origo1.anchorX();

			if( w1 > w2 )
				w = w1;
			else
				w = w2;

			// Height

			float h1 = h * pChi->m_origo1.hotspotY();
			float h2 = h - h1;

			h1 -= pChi->m_y1;
			h2 += pChi->m_y1;

			if( h1 < 0.f )
				h1 = 0.f;
			if( h2 < 0.f )
				h2 = 0.f;

			if( pChi->m_origo1.anchorY() != 0.f )
				h1 /= pChi->m_origo1.anchorY();

			if( pChi->m_origo1.anchorY() != 1.f )
				h2 /= 1.f - pChi->m_origo1.anchorY();

			if( h1 > h2 )
				h = h1;
			else
				h = h2;
		}

		if( w > 2000 )
			int x = 0;

		if( w > minW )
			minW = w;
		if( h > minH )
			minH = h;

		pChi = pChi->m_pNextSibling;
	}

	// Make sure everything is OK.

//	if( w > maxWidth || h > maxHeight)
//		return	false;																// Error! Can't make widget big enough!

	// Set minWidth & minHeight


	double fAddValue = 0.5;


	int roundMinW = (int) (minW + fAddValue);
	int roundMinH = (int) (minH + fAddValue);
	int roundMaxW = (int) maxW;
	int roundMaxH = (int) maxH;

	if( m_sizeLimit_tree.minW != roundMinW || m_sizeLimit_tree.minH != roundMinH
		|| m_sizeLimit_tree.maxW != roundMaxW || m_sizeLimit_tree.maxH != roundMaxH )
	{

		m_sizeLimit_tree.minW = roundMinW;
		m_sizeLimit_tree.minH = roundMinH;
		m_sizeLimit_tree.maxW = roundMaxW;
		m_sizeLimit_tree.maxH = roundMaxH;
		RefreshSizeLimit();
	}
}

//____ RefreshSizeLimit() _____________________________________________________

void WgWidget::RefreshSizeLimit( void )
{

	// Combine self, tree and user.

	WgMinMax2D	sizeLimit = m_sizeLimit_self;

	sizeLimit.Union( m_sizeLimit_tree );
	sizeLimit.Union( m_sizeLimit_user );

	// Some checks...

	if( sizeLimit.maxH < m_sizeLimit.minH )
		sizeLimit.maxH = m_sizeLimit.minH;
	if( sizeLimit.maxW < m_sizeLimit.minW )
		sizeLimit.maxW = m_sizeLimit.minW;

	// Update recursively if we had a change.

	if( sizeLimit != m_sizeLimit )
	{
		m_sizeLimit = sizeLimit;

		if( m_pParent != 0 )
			m_pParent->RefreshTreeSizeLimit();
	}
}


//____ Clone() ________________________________________________________________

WgWidget * WgWidget::Clone( void )
{
	WgWidget	* pClone = NewOfMyType();

	CloneContent( pClone, pClone, 0 );

	return	pClone;
}


//____ CloneBranch() __________________________________________________________

WgWidget * WgWidget::CloneBranch( void )
{
	WgWidget* pCloneBranch = BuildCloneBranch( this, Parent() );

	CloneContent( pCloneBranch, pCloneBranch, this );

	// Connect the clone-branch to the rest of the tree

	if( m_pParent != 0 )
		pCloneBranch->SetParent( m_pParent );

	return	pCloneBranch;
}


//____ CloneContent() _________________________________________________________

void	WgWidget::CloneContent( WgWidget * _pClone, WgWidget * _pCloneRoot, WgWidget * _pTreeRoot )
{

	// Take care of the simple ones first...

	_pClone->m_origo1				= m_origo1;
	_pClone->m_origo2				= m_origo2;
	_pClone->m_x1					= m_x1;
	_pClone->m_y1					= m_y1;
	_pClone->m_x2					= m_x2;
	_pClone->m_y2					= m_y2;

	_pClone->m_geo					= m_geo;
	_pClone->m_sizeLimit 			= m_sizeLimit;
	_pClone->m_sizeLimit_user		= m_sizeLimit_user;
	_pClone->m_sizeLimit_self		= m_sizeLimit_self;
	_pClone->m_sizeLimit_tree		= m_sizeLimit_tree;
	_pClone->m_bModal 				= m_bModal;
//	_pClone->bRender = true;
	_pClone->m_bOpaque 				= m_bOpaque;
	_pClone->m_bRenderedHere 		= false;
	_pClone->m_pointerMask			= m_pointerMask;
	_pClone->m_actionFilterBlock	= m_actionFilterBlock;
	_pClone->m_actionFilterCopy 	= m_actionFilterCopy;

	_pClone->m_bEnabled				= m_bEnabled;
	_pClone->m_bHidden				= m_bHidden;

	// Clone Emitter data

	_pClone->m_nCallbacks 			= m_nCallbacks;
	if( m_nCallbacks && _pTreeRoot )
	{
		_pClone->m_paCallbacks		= new WgCallback[m_nCallbacks];
		for( int i = 0 ; i < m_nCallbacks ; i++ )
		{
			_pClone->m_paCallbacks[i] = m_paCallbacks[i];

			char	map[1024];												// Should be more than enough...
			map[0] = 0;

			if( _pTreeRoot->PathFinder( (WgWidget *) _pClone->m_paCallbacks[i].pObj, map ) )	// Typecast to WgWidget is OK in this case...
				_pClone->m_paCallbacks[i].pObj = _pCloneRoot->PathFollower( map );
		}
	}
	else
		_pClone->m_paCallbacks		= 0;

	// Clone Type-specific data.

	DoMyOwnCloning( _pClone, _pCloneRoot, _pTreeRoot );

	// Go through hierarchy recursively

	if( m_pNextSibling != 0 && _pClone->m_pNextSibling != 0 )
		m_pNextSibling->CloneContent( _pClone->m_pNextSibling, _pCloneRoot, _pTreeRoot );

	if( m_pFirstChild != 0 && _pClone->m_pFirstChild != 0 )
		m_pFirstChild->CloneContent( _pClone->m_pFirstChild, _pCloneRoot, _pTreeRoot );

}


//____ BuildCloneBranch() _____________________________________________________


WgWidget* WgWidget::BuildCloneBranch( const WgWidget * pSrc, WgWidget * pDstParent )
{
	WgWidget* pClone = pSrc->NewOfMyType();

	pClone->m_pParent = pDstParent;

	if(pDstParent->m_pFirstChild == 0)
		pDstParent->m_pFirstChild = pClone;

	if(pDstParent->m_pLastChild)
	{
		pClone->m_pPrevSibling = pDstParent->m_pLastChild;
		pClone->m_pPrevSibling->m_pNextSibling = pClone;
	}

	pDstParent->m_pLastChild = pClone;


	for(WgWidget* pSrcChild = pSrc->FirstChild(); pSrcChild; pSrcChild = pSrcChild->NextSibling())
		BuildCloneBranch(pSrcChild, pClone);
											 
	return pClone;
}
/*
void	WgWidget::BuildCloneBranch( WgWidget ** _wpAddr, WgWidget * _pCloneParent )
{
	WgWidget	* pClone;

	pClone = NewOfMyType();
	* _wpAddr = pClone;

	pClone->m_pParent = _pCloneParent;

	if( m_pFirstChild != 0 )
		m_pFirstChild->BuildCloneBranch( &pClone->m_pFirstChild, pClone );

	if( m_pNextSibling != 0 && _pCloneParent != m_pParent )
		m_pNextSibling->BuildCloneBranch( &pClone->m_pNextSibling, _pCloneParent );
}*/


//____ PathFinder() ___________________________________________________________

bool WgWidget::PathFinder( const WgWidget * _goal, char * _pMapEntry ) const
{
	if( _goal == this )
	{
		_pMapEntry[0] = 0;
		return	true;
	}

	if( m_pParent != 0 && m_pNextSibling != 0 )		// m_pParent != 0: Ugly way to see if we are CloneRoot.
	{
		_pMapEntry[0] = 's';
		_pMapEntry[1] = 0;
		if( m_pNextSibling->PathFinder( _goal, _pMapEntry+1 ) )
			return	true;
	}

	if( m_pFirstChild != 0 )
	{
		_pMapEntry[0] = 'c';
		_pMapEntry[1] = 0;
		if( m_pFirstChild->PathFinder( _goal, _pMapEntry+1 ) )
			return	true;
	}

	return	false;
}

//____ PathFollower() _________________________________________________________

WgWidget * WgWidget::PathFollower( const char * _pMap ) const
{
	const WgWidget * pWidget = this;

 	while( * _pMap != 0 )
 	{
 		if( * _pMap == 'c' )
 			pWidget = pWidget->m_pFirstChild;
 		else if( * _pMap == 's' )
 			pWidget = pWidget->m_pNextSibling;
 		_pMap++;
 	}
	return	(WgWidget *) pWidget;
}

//____ DoMyOwnRefresh() _______________________________________________________

void WgWidget::DoMyOwnRefresh( void )
{
}

//____ DoMyOwnUpdate() ________________________________________________________

void WgWidget::DoMyOwnUpdate( const WgUpdateInfo& _info )
{
}

//____ DoMyOwnActionRespond() _________________________________________________

void WgWidget::DoMyOwnActionRespond( WgInput::UserAction _action, int _button_key, const WgActionDetails& _info, const WgInput& _inputObj )
{
}

//____ DoMyOwnRender() ________________________________________________________

void WgWidget::DoMyOwnRender( const WgRect& _window, const WgRect& _clip, Uint8 _layer )
{
}

//____ DoMyOwnCloning() _______________________________________________________

void WgWidget::DoMyOwnCloning( WgWidget * _pClone, const WgWidget * _pCloneRoot, const WgWidget * _pBranchRoot )
{
}


//____ DoMyOwnDisOrEnable() ___________________________________________________

void WgWidget::DoMyOwnDisOrEnable( void )
{
	RequestRender( 0, 0, m_geo.w, m_geo.h, m_pFirstChild, true );
}

//____ DoMyOwnGeometryChange() ________________________________________________

void WgWidget::DoMyOwnGeometryChange( WgRect& oldGeo, WgRect& newGeo )
{
}


//____ WgUpdateInfo::WgUpdateInfo() ____________________________________________

WgUpdateInfo::WgUpdateInfo( void )
{
	bQuit = false;
	bKbd = bMouse = bActive = true;
	msTime = msDiff = 0;
}
