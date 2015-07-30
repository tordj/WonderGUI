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
#ifndef WG_PACKPANEL_DOT_H
#define WG_PACKPANEL_DOT_H

#ifndef	WG_SIZEBROKER_DOT_H
#	include <wg_sizebroker.h>
#endif

#ifndef WG_VECTORPANEL_DOT_H
#	include <wg_vectorpanel.h>
#endif

class WgPackPanel;
typedef	WgStrongPtr<WgPackPanel,WgVectorPanelPtr>		WgPackPanelPtr;
typedef	WgWeakPtr<WgPackPanel,WgVectorPanelWeakPtr>	WgPackPanelWeakPtr;

class WgPackHook;
typedef	WgHookTypePtr<WgPackHook,WgVectorHookPtr>	WgPackHookPtr;

class WgPackHook : public WgVectorHook
{
	friend class WgPackPanel;

public:
	virtual bool			isInstanceOf( const char * pClassName ) const;
	virtual const char *	className( void ) const;
	static const char		CLASSNAME[];
	static WgPackHookPtr	cast( const WgHookPtr& pInterface );
	
	bool	setWeight( float weight );
	float	weight() { return m_weight; }

	WgPackHookPtr	prev() const { return _prev(); }
	WgPackHookPtr	next() const { return _next(); }
	WgPackPanelPtr	parent() const;

protected:
	PROTECTED_LINK_METHODS( WgPackHook );

	WgPackHook( WgPackPanel * pParent );

	WgContainer * _parent() const;
	
	float			m_weight;			// Weight for space allocation.
	WgRect			m_geo;				// Real geo of child (no padding included).
	WgSize			m_preferredSize;	// Cached padded preferred size from the child.
	WgPackPanel *	m_pParent;
};


/**
 * @brief	A widget for arranging children horizontally or vertically.
 *
 * A widget for arranging children horizontally or vertically.
 */

class WgPackPanel : public WgVectorPanel
{
	friend class WgPackHook;

public:
	static WgPackPanelPtr	create() { return WgPackPanelPtr(new WgPackPanel()); }
	
	bool		isInstanceOf( const char * pClassName ) const;
	const char *className( void ) const;
	static const char	CLASSNAME[];
	static WgPackPanelPtr	cast( const WgObjectPtr& pObject );

	inline WgPackHookPtr addWidget( const WgWidgetPtr& pWidget ) { return static_cast<WgPackHook*>(WgVectorPanel::_addWidget(pWidget.rawPtr())); }
	inline WgPackHookPtr insertWidget( const WgWidgetPtr& pWidget, const WgWidgetPtr& pSibling ) { return static_cast<WgPackHook*>(WgVectorPanel::_insertWidget(pWidget.rawPtr(),pSibling.rawPtr())); }
    
	void			setOrientation( WgOrientation orientaiton );
	WgOrientation	orientation() const { return m_bHorizontal?WG_HORIZONTAL:WG_VERTICAL; }
	
	WgPackHookPtr	firstHook() const { return static_cast<WgPackHook*>(_firstHook()); }
	WgPackHookPtr	lastHook() const { return static_cast<WgPackHook*>(_lastHook()); }

	void			setSizeBroker( const WgSizeBrokerPtr& pBroker );
	WgSizeBrokerPtr	sizeBroker() const { return m_pSizeBroker; }

	int				matchingHeight( int width ) const;
	int				matchingWidth( int height ) const;

	WgSize			preferredSize() const;
	
protected:
	WgPackPanel();
	virtual ~WgPackPanel();
	virtual WgWidget* _newOfMyType() const { return new WgPackPanel(); };

    // Overloaded from Widget
    
	void			_onNewSize( const WgSize& size );
 
    
	// Overloaded from Container
	
	WgHook*			_firstHookWithGeo( WgRect& geo ) const;
	WgHook*			_nextHookWithGeo( WgRect& geo, WgHook * pHook ) const;
	
	WgHook*			_lastHookWithGeo( WgRect& geo ) const;
	WgHook*			_prevHookWithGeo( WgRect& geo, WgHook * pHook ) const;
	
	
	// Overloaded from VectorPanel
	
	WgRect			_hookGeo( const WgVectorHook * pHook );
	void			_onResizeRequested( WgVectorHook * pHook );
	void			_onRenderRequested( WgVectorHook * pHook );
	void			_onRenderRequested( WgVectorHook * pHook, const WgRect& rect );
	void			_onWidgetAppeared( WgVectorHook * pInserted );				// so parent can update geometry and possibly request render.
	void			_onWidgetDisappeared( WgVectorHook * pToBeRemoved );		// so parent can update geometry and possibly request render.
	void			_onWidgetsReordered();
	void			_refreshAllWidgets();
	WgVectorHook *	_newHook();
	
	//
	
	inline WgPackHook *	_firstHook() const { return static_cast<WgPackHook*>(m_hooks.first()); }
	inline WgPackHook *	_lastHook() const { return static_cast<WgPackHook*>(m_hooks.last()); }


	void			_refreshChildGeo();
	void			_updatePreferredSize();
	int				_populateSizeBrokerArray( WgSizeBrokerItem * pArray ) const;
	int				_populateSizeBrokerArray( WgSizeBrokerItem * pArray, int forcedBreadth ) const;


	bool			m_bHorizontal;
	WgSizeBrokerPtr	m_pSizeBroker;
	WgSize			m_preferredSize;

};


#endif //WG_PACKPANEL_DOT_H
