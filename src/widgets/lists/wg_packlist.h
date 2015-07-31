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
	
	class PackList;
	typedef	WgStrongPtr<PackList,List_p>		PackList_p;
	typedef	WgWeakPtr<PackList,List_wp>	PackList_wp;
	
	class PackListHook;
	typedef	HookTypePtr<PackListHook,ListHook_p>	PackListHook_p;
	
	class PackListHook : public ListHook
	{
		friend class PackList;
		friend class HookArray<PackListHook>;
	public:
		virtual bool				isInstanceOf( const char * pClassName ) const;
		virtual const char *		className( void ) const;
		static const char			CLASSNAME[];
		static PackListHook_p	cast( const Hook_p& pInterface );
	
		Coord			pos() const;
		Size			size() const;
		Rect			geo() const;
		Coord			globalPos() const;
		Rect			globalGeo() const;
	
		PackListHook_p	prev() const { return static_cast<PackListHook*>(_prevHook()); }
		PackListHook_p	next() const { return static_cast<PackListHook*>(_nextHook()); }
		PackList_p		parent() const { return m_pParent; }
	
	protected:
		PackListHook() {};
	
		void			_requestRender();
		void			_requestRender( const Rect& rect );
		void			_requestResize();
	
		Hook *		_prevHook() const;
		Hook *		_nextHook() const;
	
		Container *	_parent() const;
		
		PackList *	m_pParent;
		int				m_ofs;				// Offset in pixels for start of this list item.
		int				m_length;			// Length in pixels of this list item. Includes widget padding.
		int				m_prefBreadth;		// Prefereed breadth of this widget.
	
	};
	
	
	//____ PackList ____________________________________________________________
	
	class PackList : public List, protected ColumnHeaderHolder
	{
		friend class PackListHook;
	public:
		static PackList_p	create() { return PackList_p(new PackList()); }
	
		//____ Interfaces ______________________________________
	
		ColumnHeader			header;
	
		//____ Methods _________________________________________
	
		virtual bool			isInstanceOf( const char * pClassName ) const;
		virtual const char *	className( void ) const;
		static const char		CLASSNAME[];
		static PackList_p	cast( const Object_p& pObject );
	
		PackListHook_p		addWidget( const Widget_p& pWidget );
		PackListHook_p		insertWidget( const Widget_p& pWidget, const Widget_p& pSibling );
		PackListHook_p		insertWidgetSorted( const Widget_p& pWidget );
	
		bool					removeWidget( const Widget_p& pWidget );
		bool					clear();
	
		void					setOrientation( WgOrientation orientation );
		WgOrientation			orientation() const { return m_bHorizontal?WG_HORIZONTAL:WG_VERTICAL; }
	
		void					sortWidgets();
		void					setSortOrder( WgSortOrder order );
		WgSortOrder				getSortOrder() const { return m_sortOrder; }
	
		void					setSortFunction( WidgetSortFunc pSortFunc );
		WidgetSortFunc		sortFunction() const { return m_pSortFunc; }
	
		Size					preferredSize() const;
		int						matchingHeight( int width ) const;
		int						matchingWidth( int height ) const;
	
		bool					setMinEntrySize( Size min );
		bool					setMaxEntrySize( Size max );
		Size					minEntrySize() const { return m_minEntrySize; }
		Size					maxEntrySize() const { return m_maxEntrySize; }
	
	
	
	protected:
		PackList();
		virtual ~PackList();
		Widget*		_newOfMyType() const { return new PackList(); };
	
		void			_onCollectPatches( Patches& container, const Rect& geo, const Rect& clip );
		void			_onMaskPatches( Patches& patches, const Rect& geo, const Rect& clip, WgBlendMode blendMode );
		void			_onCloneContent( const Widget * _pOrg );
		void			_renderPatches( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, Patches * _pPatches );
		void			_onRender( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, const Rect& _clip );
		void			_onNewSize( const Size& size );
		void			_onRefresh();
		void			_onRefreshList();
	
		void			_onMsg( const Msg_p& pMsg );
		void			_onStateChanged( State oldState );
	
		void			_onRequestRender( PackListHook * pHook );
		void			_onRequestRender( PackListHook * pHook, const Rect& rect );
		void			_onRequestResize( PackListHook * pHook );
	
		void			_requestRenderChildrenFrom( PackListHook * pHook );
		void			_updateChildOfsFrom( PackListHook * pHook );
	
		void			_onWidgetAppeared( ListHook * pInserted );
		void			_onWidgetDisappeared( ListHook * pToBeRemoved );		// Call BEFORE widget is removed from m_hooks.
	
		Widget * 		_findWidget( const Coord& ofs, WgSearchMode mode );
		ListHook *	_findEntry( const Coord& ofs );
		int				_getInsertionPoint( const Widget * pWidget ) const;
		void			_getChildGeo( Rect& geo, const PackListHook * pHook ) const;
		void			_getEntryGeo( Rect& geo, const ListHook * pHook ) const;
		int				_getEntryAt( int pixelofs ) const;
		Rect			_listArea() const;
		Rect			_listWindow() const;
		Rect			_listCanvas() const;
		Rect			_headerGeo() const;
		Size			_windowPadding() const;
	
		void			_onEntrySkinChanged( Size oldPadding, Size newPadding );
		void			_onLassoUpdated( const Rect& oldLasso, const Rect& newLasso );
		void			_refreshHeader();
		bool			_sortEntries();
	
		Object *		_object() { return this; }
		void			_onFieldDirty(Field * pField);
		void			_onFieldResize(Field * pField);
	
		Size			_paddedLimitedPreferredSize( Widget * pChild );
		int				_paddedLimitedMatchingHeight( Widget * pChild, int paddedWidth );
		int				_paddedLimitedMatchingWidth( Widget * pChild, int paddedHeight );
	
		Hook*			_firstHook() const;
		Hook*			_lastHook() const;
	
		Hook*			_firstHookWithGeo( Rect& geo ) const;
		Hook*			_nextHookWithGeo( Rect& geo, Hook * pHook ) const;
	
		Hook*			_lastHookWithGeo( Rect& geo ) const;
		Hook*			_prevHookWithGeo( Rect& geo, Hook * pHook ) const;
	
		ColumnHeaderField	m_header;
	
		bool				m_bHorizontal;
	
		WgSortOrder			m_sortOrder;
		WidgetSortFunc	m_pSortFunc;
	
		HookArray<PackListHook>	m_hooks;
	
		int					m_contentBreadth;
		int					m_contentLength;
		Size				m_size;
	
		Size				m_entryPadding;
		Size				m_minEntrySize;
		Size				m_maxEntrySize;
	
		//----
	
		void			_addToContentPreferredSize( int length, int breadth );
		void			_subFromContentPreferredSize( int length, int breadth );
	
		int				m_contentPreferredLength;
		int				m_contentPreferredBreadth;
		int				m_nbPreferredBreadthEntries;			// Number of entries whose preferred breadth are the same as m_preferredSize.
	
	};
	
	

} // namespace wg
#endif //WG_PACKLIST_DOT_H
