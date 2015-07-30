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

#ifndef WG_STACKPANEL_DOT_H
#define WG_STACKPANEL_DOT_H

#ifndef WG_VECTORPANEL_DOT_H
#	include <wg_vectorpanel.h>
#endif

class WgStackPanel;
typedef	WgStrongPtr<WgStackPanel,WgVectorPanelPtr>		WgStackPanelPtr;
typedef	WgWeakPtr<WgStackPanel,WgVectorPanelWeakPtr>	WgStackPanelWeakPtr;

class WgStackHook;
typedef	WgHookTypePtr<WgStackHook,WgVectorHookPtr>		WgStackHookPtr;

//____ WgStackHook ____________________________________________________________

class WgStackHook : public WgVectorHook
{
	friend class WgStackPanel;
public:
	virtual bool			isInstanceOf( const char * pClassName ) const;
	virtual const char *	className( void ) const;
	static const char		CLASSNAME[];
	static WgStackHookPtr	cast( const WgHookPtr& pInterface );

	enum SizePolicy
	{
		DEFAULT,
		STRETCH,
		SCALE
	};

	void			setSizePolicy( SizePolicy policy );
	SizePolicy		getSizePolicy() const { return m_sizePolicy; }
		
	void			setOrigo( WgOrigo origo );
	WgOrigo	origo() const { return m_origo; }

	WgStackHookPtr	prev() const { return _prev(); }
	WgStackHookPtr 	next() const { return _next(); }

	WgStackPanelPtr 	parent() const { return m_pParent; }

protected:
	PROTECTED_LINK_METHODS( WgStackHook );

	WgStackHook( WgStackPanel * pParent );

	WgContainer * _parent() const;

	WgRect			_getGeo( const WgRect& parentGeo ) const;

	WgStackPanel *	m_pParent;
	
	WgOrigo	m_origo;
	SizePolicy		m_sizePolicy;
	
};

//____ WgStackPanel ___________________________________________________________

class WgStackPanel : public WgVectorPanel
{
friend class WgStackHook;	
	
public:
	static WgStackPanelPtr	create() { return WgStackPanelPtr(new WgStackPanel()); }

	bool		isInstanceOf( const char * pClassName ) const;
	const char *className( void ) const;
	static const char	CLASSNAME[];
	static WgStackPanelPtr	cast( const WgObjectPtr& pObject );
	
	inline WgStackHookPtr addWidget( const WgWidgetPtr& pWidget ) { return static_cast<WgStackHook*>(WgVectorPanel::_addWidget(pWidget.rawPtr())); }
	inline WgStackHookPtr insertWidget( const WgWidgetPtr& pWidget, const WgWidgetPtr& pSibling ) { return static_cast<WgStackHook*>(WgVectorPanel::_insertWidget(pWidget.rawPtr(),pSibling.rawPtr())); }

	inline WgStackHookPtr firstHook() const { return static_cast<WgStackHook*>(m_hooks.first()); }
	inline WgStackHookPtr lastHook() const { return static_cast<WgStackHook*>(m_hooks.last()); }

	// Overloaded from WgWidget

	int		matchingHeight( int width ) const;
	int		matchingWidth( int height ) const;

	WgSize	preferredSize() const;

protected:
	WgStackPanel();
	virtual ~WgStackPanel();
	virtual WgWidget* _newOfMyType() const { return new WgStackPanel(); };

	// Overloaded from Widget

//	void	_onCloneContent( const WgWidget * _pOrg );
	void	_onNewSize( const WgSize& size );

	// Overloaded from WgPanel

	WgHook *	_firstHookWithGeo( WgRect& writeGeo ) const;
	WgHook *	_nextHookWithGeo( WgRect& writeGeo, WgHook * pHook ) const;

	WgHook *	_lastHookWithGeo( WgRect& writeGeo ) const;
	WgHook *	_prevHookWithGeo( WgRect& writeGeo, WgHook * pHook ) const;

	// Overloaded from WgVectorPanel

	WgRect	_hookGeo( const WgVectorHook * pHook );
	void	_onResizeRequested( WgVectorHook * pHook );
	void	_onRenderRequested( WgVectorHook * pHook );
	void	_onRenderRequested( WgVectorHook * pHook, const WgRect& rect );
	void	_onWidgetAppeared( WgVectorHook * pInserted );				// so parent can update geometry and possibly request render.
	void	_onWidgetDisappeared( WgVectorHook * pToBeRemoved );		// so parent can update geometry and possibly request render.
	void	_onWidgetsReordered();
	void	_refreshAllWidgets();
	WgVectorHook * _newHook();

	// Internal to WgStackPanel

	void 	_refreshPreferredSize();
	void	_adaptChildrenToSize();
	void	_renderFromChildOnward( WgVectorHook * pHook );

	inline WgStackHook *	_firstHook() const { return static_cast<WgStackHook*>(m_hooks.first()); }
	inline WgStackHook *	_lastHook() const { return static_cast<WgStackHook*>(m_hooks.last()); }


	WgSize	m_size;
	WgSize	m_preferredSize;

};



#endif //WG_STACKPANEL_DOT_H
