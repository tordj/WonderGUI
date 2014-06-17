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
#ifndef WG_PACKLIST_DOT_H
#define WG_PACKLIST_DOT_H

#ifndef WG_LIST_DOT_H
#	include <wg_list.h>
#endif

#ifndef WG_HOOKARRAY_DOT_H
#	include <wg_hookarray.h>
#endif

#ifndef WG_ICONFIELD_DOT_H
#	include <wg_iconfield.h>
#endif

#ifndef WG_TEXTFIELD_DOT_H
#	include <wg_textfield.h>
#endif

class WgPackList;
typedef	WgStrongPtr<WgPackList,WgListPtr>		WgPackListPtr;
typedef	WgWeakPtr<WgPackList,WgListWeakPtr>	WgPackListWeakPtr;

class WgPackListHook;
typedef	WgHookTypePtr<WgPackListHook,WgListHookPtr>	WgPackListHookPtr;

class WgPackListHook : public WgListHook
{
	friend class WgPackList;
	friend class WgHookArray<WgPackListHook>;
public:
	virtual bool				IsInstanceOf( const char * pClassName ) const;
	virtual const char *		ClassName( void ) const;
	static const char			CLASSNAME[];
	static WgPackListHookPtr	Cast( const WgHookPtr& pInterface );

	WgCoord			Pos() const;
	WgSize			Size() const;
	WgRect			Geo() const;
	WgCoord			GlobalPos() const;
	WgRect			GlobalGeo() const;

	WgPackListHookPtr	Prev() const { return static_cast<WgPackListHook*>(_prevHook()); }
	WgPackListHookPtr	Next() const { return static_cast<WgPackListHook*>(_nextHook()); }
	WgPackListPtr		Parent() const { return m_pParent; }

protected:
	WgPackListHook() {};

	void			_requestRender();
	void			_requestRender( const WgRect& rect );
	void			_requestResize();

	WgHook *		_prevHook() const;
	WgHook *		_nextHook() const;

	WgContainer *	_parent() const;
	
	WgPackList *	m_pParent;
	int				m_ofs;				// Offset in pixels for start of this list item.
	int				m_length;			// Length in pixels of this list item. Includes widget padding.
	int				m_prefBreadth;		// Prefereed breadth of this widget.

};




//____ WgPackList ____________________________________________________________

class WgPackList : public WgList
{
	friend class WgPackListHook;
public:
	static WgPackListPtr	Create() { return WgPackListPtr(new WgPackList()); }
	
	virtual bool			IsInstanceOf( const char * pClassName ) const;
	virtual const char *	ClassName( void ) const;
	static const char		CLASSNAME[];
	static WgPackListPtr	Cast( const WgObjectPtr& pObject );

	WgPackListHookPtr		AddWidget( const WgWidgetPtr& pWidget );
	WgPackListHookPtr		InsertWidget( const WgWidgetPtr& pWidget, const WgWidgetPtr& pSibling );
	WgPackListHookPtr		InsertWidgetSorted( const WgWidgetPtr& pWidget );

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
	int						HeightForWidth( int width ) const;
	int						WidthForHeight( int height ) const;

	bool					SetMinEntrySize( WgSize min );
	bool					SetMaxEntrySize( WgSize max );
	WgSize					MinEntrySize() const { return m_minEntrySize; }
	WgSize					MaxEntrySize() const { return m_maxEntrySize; }

	class Header : private WgIconHolder, private WgTextHolder
	{
		friend class WgPackList;
	public:
		WgIconField			icon;
		WgIconField			arrow;
		WgTextField			label;

		void				SetSkin( const WgSkinPtr& pSkin );
		inline WgSkinPtr	Skin() const { return m_pSkin; }

		inline WgIIconPtr		Icon() { return WgIIconPtr(m_pHolder, &icon); }
		inline WgIIconPtr		Arrow() { return WgIIconPtr(m_pHolder, &arrow); }
		inline WgIModifTextPtr	Label() { return WgIModifTextPtr(m_pHolder, &label); }
	private:
		void				_fieldModified( WgTextField * pText );
		void				_fieldModified( WgIconField * pField );

		WgPackList *	m_pHolder;
		WgSkinPtr			m_pSkin;
		int					m_height;
		int					m_width;
		WgState				m_state;
		bool				m_bPressed;
	};

	Header		header;

protected:
	WgPackList();
	virtual ~WgPackList();
	WgWidget*		_newOfMyType() const { return new WgPackList(); };

	void			_onCollectPatches( WgPatches& container, const WgRect& geo, const WgRect& clip );
	void			_onMaskPatches( WgPatches& patches, const WgRect& geo, const WgRect& clip, WgBlendMode blendMode );
	void			_onCloneContent( const WgWidget * _pOrg );
	void			_renderPatches( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, WgPatches * _pPatches );
	void			_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip );
	void			_onNewSize( const WgSize& size );
	void			_onRefresh();
	void			_onRefreshList();

	void			_onEvent( const WgEventPtr& pEvent, WgEventHandler * pHandler );
	void			_onStateChanged( WgState oldState );

	void			_onRequestRender( WgPackListHook * pHook );
	void			_onRequestRender( WgPackListHook * pHook, const WgRect& rect );
	void			_onRequestResize( WgPackListHook * pHook );

	void			_requestRenderChildrenFrom( WgPackListHook * pHook );
	void			_updateChildOfsFrom( WgPackListHook * pHook );

	void			_onWidgetAppeared( WgListHook * pInserted );
	void			_onWidgetDisappeared( WgListHook * pToBeRemoved );		// Call BEFORE widget is removed from m_hooks.

	WgWidget * 		_findWidget( const WgCoord& ofs, WgSearchMode mode );
	WgListHook *	_findEntry( const WgCoord& ofs );
	int				_getInsertionPoint( const WgWidget * pWidget ) const;
	void			_getChildGeo( WgRect& geo, const WgPackListHook * pHook ) const;
	void			_getEntryGeo( WgRect& geo, const WgListHook * pHook ) const;
	int				_getEntryAt( int pixelofs ) const;
	WgRect			_listArea() const;
	WgRect			_listWindow() const;
	WgRect			_listCanvas() const;
	WgRect			_headerGeo() const;
	WgSize			_windowPadding() const;

	void			_onEntrySkinChanged( WgSize oldPadding, WgSize newPadding );
	void			_onLassoUpdated( const WgRect& oldLasso, const WgRect& newLasso );
	void			_refreshHeader();
	bool			_sortEntries();

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

	WgHookArray<WgPackListHook>	m_hooks;

	int					m_contentBreadth;
	int					m_contentLength;
	WgSize				m_size;

	WgSize				m_entryPadding;
	WgSize				m_minEntrySize;
	WgSize				m_maxEntrySize;

	//----

	void			_addToContentPreferredSize( int length, int breadth );
	void			_subFromContentPreferredSize( int length, int breadth );

	int				m_contentPreferredLength;
	int				m_contentPreferredBreadth;
	int				m_nbPreferredBreadthEntries;			// Number of entries whose preferred breadth are the same as m_preferredSize.

};


#endif //WG_PACKLIST_DOT_H
