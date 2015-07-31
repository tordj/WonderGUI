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

#ifndef WG_COLUMNHEADER_DOT_H
#	include <wg_columnheader.h>
#endif

namespace wg 
{
	
	class WgPackList;
	typedef	WgStrongPtr<WgPackList,WgList_p>		WgPackList_p;
	typedef	WgWeakPtr<WgPackList,WgList_wp>	WgPackList_wp;
	
	class WgPackListHook;
	typedef	WgHookTypePtr<WgPackListHook,WgListHook_p>	WgPackListHook_p;
	
	class WgPackListHook : public WgListHook
	{
		friend class WgPackList;
		friend class WgHookArray<WgPackListHook>;
	public:
		virtual bool				isInstanceOf( const char * pClassName ) const;
		virtual const char *		className( void ) const;
		static const char			CLASSNAME[];
		static WgPackListHook_p	cast( const WgHook_p& pInterface );
	
		WgCoord			pos() const;
		WgSize			size() const;
		WgRect			geo() const;
		WgCoord			globalPos() const;
		WgRect			globalGeo() const;
	
		WgPackListHook_p	prev() const { return static_cast<WgPackListHook*>(_prevHook()); }
		WgPackListHook_p	next() const { return static_cast<WgPackListHook*>(_nextHook()); }
		WgPackList_p		parent() const { return m_pParent; }
	
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
	
	class WgPackList : public WgList, protected WgColumnHeaderHolder
	{
		friend class WgPackListHook;
	public:
		static WgPackList_p	create() { return WgPackList_p(new WgPackList()); }
	
		//____ Interfaces ______________________________________
	
		WgColumnHeader			header;
	
		//____ Methods _________________________________________
	
		virtual bool			isInstanceOf( const char * pClassName ) const;
		virtual const char *	className( void ) const;
		static const char		CLASSNAME[];
		static WgPackList_p	cast( const WgObject_p& pObject );
	
		WgPackListHook_p		addWidget( const WgWidget_p& pWidget );
		WgPackListHook_p		insertWidget( const WgWidget_p& pWidget, const WgWidget_p& pSibling );
		WgPackListHook_p		insertWidgetSorted( const WgWidget_p& pWidget );
	
		bool					removeWidget( const WgWidget_p& pWidget );
		bool					clear();
	
		void					setOrientation( WgOrientation orientation );
		WgOrientation			orientation() const { return m_bHorizontal?WG_HORIZONTAL:WG_VERTICAL; }
	
		void					sortWidgets();
		void					setSortOrder( WgSortOrder order );
		WgSortOrder				getSortOrder() const { return m_sortOrder; }
	
		void					setSortFunction( WgWidgetSortFunc pSortFunc );
		WgWidgetSortFunc		sortFunction() const { return m_pSortFunc; }
	
		WgSize					preferredSize() const;
		int						matchingHeight( int width ) const;
		int						matchingWidth( int height ) const;
	
		bool					setMinEntrySize( WgSize min );
		bool					setMaxEntrySize( WgSize max );
		WgSize					minEntrySize() const { return m_minEntrySize; }
		WgSize					maxEntrySize() const { return m_maxEntrySize; }
	
	
	
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
	
		void			_onMsg( const WgMsg_p& pMsg );
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
	
		WgObject *		_object() { return this; }
		void			_onFieldDirty(WgField * pField);
		void			_onFieldResize(WgField * pField);
	
		WgSize			_paddedLimitedPreferredSize( WgWidget * pChild );
		int				_paddedLimitedMatchingHeight( WgWidget * pChild, int paddedWidth );
		int				_paddedLimitedMatchingWidth( WgWidget * pChild, int paddedHeight );
	
		WgHook*			_firstHook() const;
		WgHook*			_lastHook() const;
	
		WgHook*			_firstHookWithGeo( WgRect& geo ) const;
		WgHook*			_nextHookWithGeo( WgRect& geo, WgHook * pHook ) const;
	
		WgHook*			_lastHookWithGeo( WgRect& geo ) const;
		WgHook*			_prevHookWithGeo( WgRect& geo, WgHook * pHook ) const;
	
		WgColumnHeaderField	m_header;
	
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
	
	

} // namespace wg
#endif //WG_PACKLIST_DOT_H
