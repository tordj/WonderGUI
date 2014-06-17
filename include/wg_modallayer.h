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
typedef	WgStrongPtr<WgModalLayer,WgLayerPtr>	WgModalLayerPtr;
typedef	WgWeakPtr<WgModalLayer,WgLayerWeakPtr>	WgModalLayerWeakPtr;

class WgModalHook;
typedef	WgHookTypePtr<WgModalHook,WgLayerHookPtr>	WgModalHookPtr;

class WgModalHook : public WgLayerHook, protected WgLink
{
	friend class WgModalLayer;
	friend class WgChain<WgModalHook>;

public:
	virtual bool			IsInstanceOf( const char * pClassName ) const;
	virtual const char *	ClassName( void ) const;
	static const char		CLASSNAME[];
	static WgModalHookPtr	Cast( const WgHookPtr& pInterface );

	void	Top();								// Put us ontop of all our siblings.

	bool	SetGeo( const WgRect& geometry, WgOrigo origo = WG_NORTHWEST );
	bool	SetGeo( const WgCoord& ofs, WgOrigo origo = WG_NORTHWEST );

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

	WgModalHookPtr	Prev() const { return _prev(); }
	WgModalHookPtr	Next() const { return _next(); }

	WgModalLayerPtr	Parent() const;

protected:
	// TODO: Constructor should in the future call SetHook() on Widget, once we are totally rid of widgets...

	PROTECTED_LINK_METHODS( WgModalHook );

	WgModalHook( WgModalLayer * pParent );

	bool		_refreshRealGeo();	// Return false if we couldn't get exactly the requested (floating) geometry.
	void		_requestResize();

	WgLayerHook *	_prevLayerHook() const;			// Iterate through all hooks except the base hook
	WgLayerHook *	_nextLayerHook() const;			// Iterate through all hooks except the base hook
	WgContainer *	_parent() const;


	WgModalLayer *	m_pParent;

	WgOrigo	m_origo;
	WgRect			m_placementGeo;		// Widgets geo relative anchor and hotspot. Setting width and height to 0 uses Widgets PreferredSize() dynamically.
										// Setting just one of them to 0 uses Widgets HeightForWidth() or WidthForHeight() dynamically.

	WgWidgetWeakPtr	m_pKeyFocus;		// Pointer at child that held focus when this modal was last on top.
};


class WgModalLayer : public WgLayer
{
	friend class WgModalHook;

public:
	static WgModalLayerPtr	Create() { return WgModalLayerPtr(new WgModalLayer()); }

	bool		IsInstanceOf( const char * pClassName ) const;
	const char *ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgModalLayerPtr	Cast( const WgObjectPtr& pObject );

	WgModalHookPtr	AddModalWidget( const WgWidgetPtr& pWidget, const WgRect& geometry, WgOrigo origo = WG_NORTHWEST );
	WgModalHookPtr	AddModalWidget( const WgWidgetPtr& pWidget, const WgCoord& pos, WgOrigo origo = WG_NORTHWEST ) { return AddModalWidget( pWidget, WgRect(pos,0,0), origo); }

	bool			RemoveModalWidgets();

	bool			RemoveWidget( const WgWidgetPtr& pWidget );
	bool			Clear();

	WgModalHookPtr	FirstModalHook();
	WgModalHookPtr	LastModalHook();


	// Overloaded from WgWidget

	int				HeightForWidth( int width ) const;
	int				WidthForHeight( int height ) const;

	WgSize			PreferredSize() const;

protected:
	WgModalLayer();
	virtual ~WgModalLayer();
	virtual WgWidget* _newOfMyType() const { return new WgModalLayer(); };


private:

	WgModalLayer *	_getModalLayer() const { return const_cast<WgModalLayer*>(this); }

	void			_updateKeyboardFocus();

	// Overloaded from WgPanel

	WgWidget *		_findWidget( const WgCoord& ofs, WgSearchMode mode );


	// Overloaded from WgLayer

	WgLayerHook *	_firstLayerHook() const { return m_modalHooks.First(); }
	WgLayerHook *	_lastLayerHook() const { return m_modalHooks.Last(); }

	//

	void			_onCloneContent( const WgWidget * _pOrg );
	void			_onNewSize( const WgSize& size );
	void			_onEvent( const WgEventPtr& pEvent, WgEventHandler * pHandler );

	WgChain<WgModalHook>	m_modalHooks;		// First modal widget lies at the bottom.

	WgWidgetWeakPtr			m_pBaseKeyFocus;

};

#endif //WG_MODALLAYER_DOT_H
