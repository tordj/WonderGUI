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
typedef	WgStrongPtr<WgModalLayer,WgLayer_p>	WgModalLayer_p;
typedef	WgWeakPtr<WgModalLayer,WgLayer_wp>	WgModalLayer_wp;

class WgModalHook;
typedef	WgHookTypePtr<WgModalHook,WgLayerHook_p>	WgModalHook_p;

class WgModalHook : public WgLayerHook, protected WgLink
{
	friend class WgModalLayer;
	friend class WgChain<WgModalHook>;

public:
	virtual bool			isInstanceOf( const char * pClassName ) const;
	virtual const char *	className( void ) const;
	static const char		CLASSNAME[];
	static WgModalHook_p	cast( const WgHook_p& pInterface );

	void	top();								// Put us ontop of all our siblings.

	bool	setGeo( const WgRect& geometry, WgOrigo origo = WG_NORTHWEST );
	bool	setGeo( const WgCoord& ofs, WgOrigo origo = WG_NORTHWEST );

	bool	setOfs( const WgCoord& ofs );
	bool	setOfsX( int x );
	bool	setOfsY( int y );

	bool	setSize( WgSize sz );
	bool	setWidth( int width );
	bool	setHeight( int height );

	bool	move( const WgCoord& ofs );
	bool	moveX( int x );
	bool	moveY( int y );


	// Standard Hook methods

	WgModalHook_p	prev() const { return _prev(); }
	WgModalHook_p	next() const { return _next(); }

	WgModalLayer_p	parent() const;

protected:
	// TODO: Constructor should in the future call setHook() on Widget, once we are totally rid of widgets...

	PROTECTED_LINK_METHODS( WgModalHook );

	WgModalHook( WgModalLayer * pParent );

	bool		_refreshRealGeo();	// Return false if we couldn't get exactly the requested (floating) geometry.
	void		_requestResize();

	WgLayerHook *	_prevLayerHook() const;			// Iterate through all hooks except the base hook
	WgLayerHook *	_nextLayerHook() const;			// Iterate through all hooks except the base hook
	WgContainer *	_parent() const;


	WgModalLayer *	m_pParent;

	WgOrigo	m_origo;
	WgRect			m_placementGeo;		// Widgets geo relative anchor and hotspot. Setting width and height to 0 uses Widgets preferredSize() dynamically.
										// Setting just one of them to 0 uses Widgets matchingHeight() or matchingWidth() dynamically.

	WgWidget_wp	m_pKeyFocus;		// Pointer at child that held focus when this modal was last on top.
};


class WgModalLayer : public WgLayer
{
	friend class WgModalHook;

public:
	static WgModalLayer_p	create() { return WgModalLayer_p(new WgModalLayer()); }

	bool		isInstanceOf( const char * pClassName ) const;
	const char *className( void ) const;
	static const char	CLASSNAME[];
	static WgModalLayer_p	cast( const WgObject_p& pObject );

	WgModalHook_p	addModalWidget( const WgWidget_p& pWidget, const WgRect& geometry, WgOrigo origo = WG_NORTHWEST );
	WgModalHook_p	addModalWidget( const WgWidget_p& pWidget, const WgCoord& pos, WgOrigo origo = WG_NORTHWEST ) { return addModalWidget( pWidget, WgRect(pos,0,0), origo); }

	bool			removeModalWidgets();

	bool			removeWidget( const WgWidget_p& pWidget );
	bool			clear();

	WgModalHook_p	firstModalHook();
	WgModalHook_p	lastModalHook();


	// Overloaded from WgWidget

	int				matchingHeight( int width ) const;
	int				matchingWidth( int height ) const;

	WgSize			preferredSize() const;

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

	WgLayerHook *	_firstLayerHook() const { return m_modalHooks.first(); }
	WgLayerHook *	_lastLayerHook() const { return m_modalHooks.last(); }

	//

	void			_onCloneContent( const WgWidget * _pOrg );
	void			_onNewSize( const WgSize& size );
	void			_onMsg( const WgMsg_p& pMsg );

	WgChain<WgModalHook>	m_modalHooks;		// First modal widget lies at the bottom.

	WgWidget_wp			m_pBaseKeyFocus;

};

#endif //WG_MODALLAYER_DOT_H
