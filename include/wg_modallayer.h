/*=========================================================================

                         >>> WonderGUI <<<

  This file is part of Tord Jansson's WonderGUI Graphics Toolkit
  and copyright (c) Tord Jansson, Sweden [tord.jansson@gmail.com].

                            -----------

  The WonderGUI Graphics Toolkit is free software you can redistribute
  this file and/or modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation either
  version 2 of the License, or (at your option) any later version.

                            -----------

  The WonderGUI Graphics Toolkit is also available for use in commercial
  closed-source projects under a separate license. Interested parties
  should contact Tord Jansson [tord.jansson@gmail.com] for details.

=========================================================================*/

#ifndef WG_MODALLAYER_DOT_H
#define WG_MODALLAYER_DOT_H

#ifndef WG_LAYER_DOT_H
#	include <wg_layer.h>
#endif


class WgModalLayer;

class WgModalHook : public WgLayerHook, protected WgLink
{
	friend class WgModalLayer;
	friend class WgChain<WgModalHook>;

public:
	const char *Type( void ) const;
	static const char * ClassType();

	void	Top();								// Put us ontop of all our siblings.

	bool	SetGeo( const WgRect& geometry, WgOrientation origo = WG_NORTHWEST );
	bool	SetGeo( const WgCoord& ofs, WgOrientation origo = WG_NORTHWEST );

	bool	SetOfs( const WgCoord& ofs );
	bool	SetOfsX( int x );
	bool	SetOfsY( int y );

	bool	SetSize( WgSize sz );
	bool	SetWidth( int width );
	bool	SetHeight( int height );

	bool	Move( const WgCoord& ofs );
	bool	MoveX( int x );
	bool	MoveY( int y );


	// Standard Hook methods

	WgModalHook *	Prev() const { return _prev(); }
	WgModalHook *	Next() const { return _next(); }

	WgModalLayer*	Parent() const;

protected:
	// TODO: Constructor should in the future call SetHook() on Widget, once we are totally rid of widgets...

	PROTECTED_LINK_METHODS( WgModalHook );

	WgModalHook( WgModalLayer * pParent );

	bool		_refreshRealGeo();	// Return false if we couldn't get exactly the requested (floating) geometry.
	void		_requestResize();

	WgHook *	_prevHook() const;
	WgHook *	_nextHook() const;
	WgWidgetHolder * _parent() const;


	WgModalLayer *	m_pParent;

	WgOrientation	m_origo;
	WgRect			m_placementGeo;		// Widgets geo relative anchor and hotspot. Setting width and height to 0 uses Widgets PreferredSize() dynamically.
										// Setting just one of them to 0 uses Widgets HeightForWidth() or WidthForHeight() dynamically.

	WgWidgetWeakPtr	m_pKeyFocus;		// Pointer at child that held focus when this modal was last on top.
};



class WgModalLayer : public WgLayer
{
	friend class WgModalHook;

public:
	WgModalLayer();
	~WgModalLayer();

	virtual const char *Type( void ) const;
	static const char * GetClass();
	virtual WgWidget * NewOfMyType() const { return new WgModalLayer(); };

	WgModalHook *	AddModal( WgWidget * pWidget, const WgRect& geometry, WgOrientation origo = WG_NORTHWEST );
	WgModalHook *	AddModal( WgWidget * pWidget, const WgCoord& pos, WgOrientation origo = WG_NORTHWEST ) { return AddModal( pWidget, WgRect(pos,0,0), origo); }

	bool			DeleteAllModal();
	bool			ReleaseAllModal();

	bool			DeleteChild( WgWidget * pWidget );
	WgWidget *		ReleaseChild( WgWidget * pWidget );

	bool			DeleteAllChildren();
	bool			ReleaseAllChildren();

	WgModalHook *	FirstModal();
	WgModalHook *	LastModal();


	// Overloaded from WgWidget

	int				HeightForWidth( int width ) const;
	int				WidthForHeight( int height ) const;

	WgSize			PreferredSize() const;

	// Overloaded from WgPanel

	WgWidget *		FindWidget( const WgCoord& ofs, WgSearchMode mode );

private:

	WgModalLayer *	_getModalLayer() const { return const_cast<WgModalLayer*>(this); }

	void			_updateKeyboardFocus();

	// Overloaded from WgLayer

	WgLayerHook *	_firstLayerHook() const { return m_modalHooks.First(); }

	//

	void			_onCloneContent( const WgWidget * _pOrg );
	void			_onNewSize( const WgSize& size );
	void			_onEvent( const WgEvent::Event * pEvent, WgEventHandler * pHandler );

	WgHook*		_firstHook() const;		// Fist Hook returned is the normal child, then follows the modal ones.
	WgHook*		_lastHook() const;		//

	WgHook *	_firstHookWithGeo( WgRect& geo ) const;
	WgHook *	_nextHookWithGeo( WgRect& geo, WgHook * pHook ) const;

	WgHook *	_lastHookWithGeo( WgRect& geo ) const;
	WgHook *	_prevHookWithGeo( WgRect& geo, WgHook * pHook ) const;

	WgChain<WgModalHook>	m_modalHooks;		// First modal widget lies at the bottom.

	WgWidgetWeakPtr			m_pBaseKeyFocus;

};

#endif //WG_MODALLAYER_DOT_H
