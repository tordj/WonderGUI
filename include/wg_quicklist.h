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

#ifndef WG_LIST_DOT_H
#	include <wg_list.h>
#endif

#ifndef WG_HOOKARRAY_DOT_H
#	include <wg_hookarray.h>
#endif

class WgQuickList;
typedef	WgSmartPtr<WgQuickList,WgListPtr>		WgQuickListPtr;
typedef	WgWeakPtr<WgQuickList,WgListWeakPtr>	WgQuickListWeakPtr;

class WgQuickListHook;
typedef	WgHookTypePtr<WgQuickListHook,WgListHookPtr>	WgQuickListHookPtr;

class WgQuickListHook : public WgListHook
{
	friend class WgQuickList;
	friend class WgHookArray<WgQuickListHook>;
public:
	virtual bool				IsInstanceOf( const char * pClassName ) const;
	virtual const char *		ClassName( void ) const;
	static const char			CLASSNAME[];
	static WgQuickListHookPtr	Cast( const WgHookPtr& pInterface );

	WgCoord			Pos() const;
	WgSize			Size() const;
	WgRect			Geo() const;
	WgCoord			ScreenPos() const;
	WgRect			ScreenGeo() const;

	WgQuickListHookPtr	Prev() const { return static_cast<WgQuickListHook*>(_prevHook()); }
	WgQuickListHookPtr	Next() const { return static_cast<WgQuickListHook*>(_nextHook()); }
	WgQuickListPtr		Parent() const { return m_pParent; }

protected:
	WgQuickListHook() {};

	void			_requestRender();
	void			_requestRender( const WgRect& rect );
	void			_requestResize();

	WgHook *		_prevHook() const;
	WgHook *		_nextHook() const;

	WgContainer *	_parent() const;
	
	WgQuickList *	m_pParent;
	int				m_ofs;				// Offset in pixels for start of this list item.
	int				m_length;			// Length in pixels of this list item. Includes widget padding.

};




//____ WgQuickList ____________________________________________________________

class WgQuickList : public WgList
{
	friend class WgQuickListHook;
public:
	static WgQuickListPtr	Create() { return WgQuickListPtr(new WgQuickList()); }
	
	virtual bool			IsInstanceOf( const char * pClassName ) const;
	virtual const char *	ClassName( void ) const;
	static const char		CLASSNAME[];
	static WgQuickListPtr	Cast( const WgObjectPtr& pObject );

	WgQuickListHookPtr		AddWidget( const WgWidgetPtr& pWidget );
	WgQuickListHookPtr		InsertWidget( const WgWidgetPtr& pWidget, const WgWidgetPtr& pSibling );
	WgQuickListHookPtr		InsertWidgetSorted( const WgWidgetPtr& pWidget );

	bool					RemoveWidget( const WgWidgetPtr& pWidget );
	bool					Clear();

	void					SetOrientation( WgOrientation orientaiton );
	WgOrientation			Orientation() const { return m_bHorizontal?WG_HORIZONTAL:WG_VERTICAL; }

	void					SortWidgets();
	void					SetSortOrder( WgSortOrder order );
	WgSortOrder				GetSortOrder() const { return m_sortOrder; }

	void					SetSortFunction( WgWidgetSortFunc pSortFunc );
	WgWidgetSortFunc		SortFunction() const { return m_pSortFunc; }

	WgSize					PreferredSize() const;

protected:
	WgQuickList();
	virtual ~WgQuickList();
	WgWidget*		_newOfMyType() const { return new WgQuickList(); };

	void			_onCollectPatches( WgPatches& container, const WgRect& geo, const WgRect& clip );
	void			_onMaskPatches( WgPatches& patches, const WgRect& geo, const WgRect& clip, WgBlendMode blendMode );
	void			_onCloneContent( const WgWidget * _pOrg );
	void			_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip );
	void			_onNewSize( const WgSize& size );
	void			_onRefresh();

	void			_onEvent( const WgEventPtr& pEvent, WgEventHandler * pHandler );
	void			_onStateChanged( WgState oldState, WgState newState );

	void			_onRequestRender( WgQuickListHook * pHook );
	void			_onRequestRender( WgQuickListHook * pHook, const WgRect& rect );
	void			_onRequestResize( WgQuickListHook * pHook );

	void			_onWidgetAppeared( WgQuickListHook * pInserted );
	void			_onWidgetDisappeared( WgQuickListHook * pToBeRemoved );		// Call BEFORE widget is removed from m_hooks.

	WgWidget * 		_findWidget( const WgCoord& ofs, WgSearchMode mode );
	void			_getChildGeo( WgRect& geo, const WgHook * pHook ) const;

	WgHook*			_firstHook() const;
	WgHook*			_lastHook() const;

	WgHook*			_firstHookWithGeo( WgRect& geo ) const;
	WgHook*			_nextHookWithGeo( WgRect& geo, WgHook * pHook ) const;

	WgHook*			_lastHookWithGeo( WgRect& geo ) const;
	WgHook*			_prevHookWithGeo( WgRect& geo, WgHook * pHook ) const;


	bool				m_bHorizontal;

	WgSortOrder			m_sortOrder;
	WgWidgetSortFunc	m_pSortFunc;

	WgHookArray<WgQuickListHook>	m_hooks;

	WgSize				m_preferredSize;
	WgSize				m_size;
};


#endif //WG_QUICKLIST_DOT_H
