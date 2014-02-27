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
#ifndef WG_QUICKLIST_DOT_H
#define WG_QUICKLIST_DOT_H

#include <wg_list.h>
#include <wg_chain.h>

class WgQuickList;
typedef	WgSmartPtr<WgQuickList,WgListPtr>		WgQuickListPtr;
typedef	WgWeakPtr<WgQuickList,WgListWeakPtr>	WgQuickListWeakPtr;

class WgQuickListHook : public WgListHook, private WgLink
{
public:
	const char *Type( void ) const;
	static const char * ClassType();
	
	WgQuickListHook *	Prev() const { return _prev(); }
	WgQuickListHook *	Next() const { return _next(); }
	WgQuickListPtr		Parent() const;

protected:
	PROTECTED_LINK_METHODS( WgQuickListHook );
	WgQuickListHook( WgQuickList * pParent );

	void	_requestRender();
	void	_requestRender( const WgRect& rect );
	void	_requestResize();

	WgHook *	_prevHook() const;
	WgHook *	_nextHook() const;

	WgContainer * _parent() const;
	
	WgQuickList *	m_pParent;
};




//____ WgQuickList ____________________________________________________________

class WgQuickList : public WgList
{
public:
	static WgQuickListPtr	Create() { return WgQuickListPtr(new WgQuickList()); }
	
	bool		IsInstanceOf( const char * pClassName ) const;
	const char *ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgQuickListPtr	Cast( const WgObjectPtr& pObject );

	WgQuickListHook *	AddChild( const WgWidgetPtr& pWidget );
	WgQuickListHook *	InsertChild( const WgWidgetPtr& pWidget, const WgWidgetPtr& pSibling );
	WgQuickListHook *	InsertChildSorted( const WgWidgetPtr& pWidget );

	void				SortChildren();
	void				SetSortOrder( WgSortOrder order );
	WgSortOrder			GetSortOrder() const { return m_sortOrder; }

	void				SetSortFunction( WgWidgetSortFunc pSortFunc );
	WgWidgetSortFunc	SortFunction() const { return m_pSortFunc; }


	WgSize				PreferredSize() const;

protected:
	WgQuickList();
	virtual ~WgQuickList();
	virtual WgWidget* _newOfMyType() const { return new WgQuickList(); };

	virtual void	_renderPatches( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, WgPatches * _pPatches );
	virtual void	_onCollectPatches( WgPatches& container, const WgRect& geo, const WgRect& clip );
	virtual void	_onMaskPatches( WgPatches& patches, const WgRect& geo, const WgRect& clip, WgBlendMode blendMode );
	virtual void	_onCloneContent( const WgWidget * _pOrg );
	virtual void	_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip );
	virtual void	_onNewSize( const WgSize& size );
	virtual void	_onRefresh();

	virtual void	_onEvent( const WgEventPtr& pEvent, WgEventHandler * pHandler );
	virtual	bool	_onAlphaTest( const WgCoord& ofs );
	virtual void	_onStateChanged( WgState oldState, WgState newState );


	WgSortOrder			m_sortOrder;
	WgWidgetSortFunc	m_pSortFunc;

};


#endif //WG_QUICKLIST_DOT_H
