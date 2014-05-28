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
#ifndef WG_STRAIGHTLIST_DOT_H
#define WG_STRAIGHTLIST_DOT_H

#ifndef WG_LIST_DOT_H
#	include <wg_list.h>
#endif

#ifndef WG_HOOKARRAY_DOT_H
#	include <wg_hookarray.h>
#endif

class WgStraightList;
typedef	WgSmartPtr<WgStraightList,WgListPtr>		WgStraightListPtr;
typedef	WgWeakPtr<WgStraightList,WgListWeakPtr>	WgStraightListWeakPtr;

class WgStraightListHook;
typedef	WgHookTypePtr<WgStraightListHook,WgListHookPtr>	WgStraightListHookPtr;

class WgStraightListHook : public WgListHook
{
	friend class WgStraightList;
	friend class WgHookArray<WgStraightListHook>;
public:
	virtual bool				IsInstanceOf( const char * pClassName ) const;
	virtual const char *		ClassName( void ) const;
	static const char			CLASSNAME[];
	static WgStraightListHookPtr	Cast( const WgHookPtr& pInterface );

	WgCoord			Pos() const;
	WgSize			Size() const;
	WgRect			Geo() const;
	WgCoord			ScreenPos() const;
	WgRect			ScreenGeo() const;

	WgStraightListHookPtr	Prev() const { return static_cast<WgStraightListHook*>(_prevHook()); }
	WgStraightListHookPtr	Next() const { return static_cast<WgStraightListHook*>(_nextHook()); }
	WgStraightListPtr		Parent() const { return m_pParent; }

protected:
	WgStraightListHook() {};

	void			_requestRender();
	void			_requestRender( const WgRect& rect );
	void			_requestResize();

	WgHook *		_prevHook() const;
	WgHook *		_nextHook() const;

	WgContainer *	_parent() const;
	
	WgStraightList *	m_pParent;
	int				m_ofs;				// Offset in pixels for start of this list item.
	int				m_length;			// Length in pixels of this list item. Includes widget padding.
	int				m_prefBreadth;		// Prefereed breadth of this widget.

};




//____ WgStraightList ____________________________________________________________

class WgStraightList : public WgList
{
	friend class WgStraightListHook;
public:
	static WgStraightListPtr	Create() { return WgStraightListPtr(new WgStraightList()); }
	
	virtual bool			IsInstanceOf( const char * pClassName ) const;
	virtual const char *	ClassName( void ) const;
	static const char		CLASSNAME[];
	static WgStraightListPtr	Cast( const WgObjectPtr& pObject );

	WgStraightListHookPtr		AddWidget( const WgWidgetPtr& pWidget );
	WgStraightListHookPtr		InsertWidget( const WgWidgetPtr& pWidget, const WgWidgetPtr& pSibling );
	WgStraightListHookPtr		InsertWidgetSorted( const WgWidgetPtr& pWidget );

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

	bool					SetMinEntrySize( WgSize min );
	bool					SetMaxEntrySize( WgSize max );
	WgSize					MinEntrySize() const { return m_minEntrySize; }
	WgSize					MaxEntrySize() const { return m_maxEntrySize; }

protected:
	WgStraightList();
	virtual ~WgStraightList();
	WgWidget*		_newOfMyType() const { return new WgStraightList(); };

	void			_onCollectPatches( WgPatches& container, const WgRect& geo, const WgRect& clip );
	void			_onMaskPatches( WgPatches& patches, const WgRect& geo, const WgRect& clip, WgBlendMode blendMode );
	void			_onCloneContent( const WgWidget * _pOrg );
	void			_renderPatches( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, WgPatches * _pPatches );
	void			_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip );
	void			_onNewSize( const WgSize& size );
	void			_onRefresh();

	void			_onEvent( const WgEventPtr& pEvent, WgEventHandler * pHandler );
	void			_onStateChanged( WgState oldState );

	void			_onRequestRender( WgStraightListHook * pHook );
	void			_onRequestRender( WgStraightListHook * pHook, const WgRect& rect );
	void			_onRequestResize( WgStraightListHook * pHook );

	void			_requestRenderChildrenFrom( WgStraightListHook * pHook );
	void			_updateChildOfsFrom( WgStraightListHook * pHook );

	void			_onWidgetAppeared( WgListHook * pInserted );
	void			_onWidgetDisappeared( WgListHook * pToBeRemoved );		// Call BEFORE widget is removed from m_hooks.

	WgWidget * 		_findWidget( const WgCoord& ofs, WgSearchMode mode );
	WgStraightListHook *_findEntry( const WgCoord& ofs );
	void			_getChildGeo( WgRect& geo, const WgStraightListHook * pHook ) const;
	void			_getEntryGeo( WgRect& geo, const WgStraightListHook * pHook ) const;
	int				_getEntryAt( int pixelofs ) const;

	bool			_onEntrySelected( WgListHook * pHook, bool bSelected, bool bPostEvent );
	int				_onRangeSelected( int firstEntry, int nbEntries, bool bSelected, bool bPostEvent );
	void			_onEntrySkinChanged( WgSize oldPadding, WgSize newPadding );

	WgSize			_paddedLimitedPreferredSize( WgWidget * pChild );
	int				_paddedLimitedHeightForWidth( WgWidget * pChild, int paddedWidth );
	int				_paddedLimitedWidthForHeight( WgWidget * pChild, int paddedHeight );

	WgHook*			_firstHook() const;
	WgHook*			_lastHook() const;

	WgHook*			_firstHookWithGeo( WgRect& geo ) const;
	WgHook*			_nextHookWithGeo( WgRect& geo, WgHook * pHook ) const;

	WgHook*			_lastHookWithGeo( WgRect& geo ) const;
	WgHook*			_prevHookWithGeo( WgRect& geo, WgHook * pHook ) const;

	bool				m_bHorizontal;

	WgSortOrder			m_sortOrder;
	WgWidgetSortFunc	m_pSortFunc;

	WgHookArray<WgStraightListHook>	m_hooks;

	int					m_contentBreadth;
	int					m_contentLength;
	WgSize				m_size;

	WgSize				m_entryPadding;
	WgSize				m_minEntrySize;
	WgSize				m_maxEntrySize;

	WgWidget *			m_pHoveredChild;	// Careful not to use the pointer! It widget might be destroyed.

	//----

	void			_addToContentPreferredSize( int length, int breadth );
	void			_subFromContentPreferredSize( int length, int breadth );

	int				m_contentPreferredLength;
	int				m_contentPreferredBreadth;
	int				m_nbPreferredBreadthEntries;			// Number of entries whose preferred breadth are the same as m_preferredSize.

};


#endif //WG_STRAIGHTLIST_DOT_H
