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

#ifndef	WDG_TREEVIEW_DOT_H
#define	WDG_TREEVIEW_DOT_H
/*
#ifndef	WG_WIDGET_DOT_H
#	include <wg_widget.h>
#endif
*/

#ifndef WDG_BASECLASS_VIEW_DOT_H
#	include <wdg_baseclass_view.h>
#endif

class Wdg_Menu;

class WgTreeEntry : protected WgLink
{
friend class Wdg_TreeView;
friend class WgChain<WgTreeEntry>;

public:

	inline WgTreeEntry * GetPrevSibling() { return Prev(); }
	inline WgTreeEntry * GetNextSibling() { return Next(); }
	inline WgTreeEntry * GetFirstChild() { return m_children.First(); }
	inline WgTreeEntry * GetLastChild() { return m_children.Last(); }
	inline WgTreeEntry * GetParent() { return pParent; }

	WgTreeEntry * GetPrev();
	WgTreeEntry * GetNext();

	inline long 	GetID() { return id; }
	inline bool		IsOpen() { return bOpen; }
	inline bool		HasChildren() { return (m_children.First()!=0)?true:false; }
	inline bool		IsSelected() { return (mode == WG_MODE_SELECTED); }
	inline bool		IsDisabled() { return (mode == WG_MODE_DISABLED); }
	inline WgItem * GetItem() { return pItem; }
	Uint32			GetLevel();

protected:
	WgTreeEntry( long _id, WgTreeEntry * _pParent, Uint16 _width, WgItem * _pItem );
	~WgTreeEntry();

	LINK_METHODS(WgTreeEntry);

	long 			id;						// ID as set by user.
	WgTreeEntry *	pParent;

	bool			bOpen;					// Set if this entry is opened, exposing enclosed entries.
	WgMode			mode;					// Mode of this entry.
	Uint16			width;					// Width of this entry alone in pixels.

	Uint16			childrenWidth;			// Width of longest enclosed and visible child/grandchild.
	Uint32			childrenHeight;			// Height of enclosed visible content.

	WgItem *		pItem;					// Pointer at WgItem if we have one.

	WgChain<WgTreeEntry>	m_children;
};



class Wdg_TreeView : public Wdg_Baseclass_View
{
	public:
		WIDGET_CONSTRUCTORS(Wdg_TreeView,Wdg_Baseclass_View)
		virtual ~Wdg_TreeView();
		virtual const char * Type() const;
		static const char * GetMyType();


		//____ Methods __________________________________________

		void	SetExpanderSource( const WgBlockSetPtr& pGfxOpen, const WgBlockSetPtr& pGfxClosed );
		void	SetIndentation( Uint8 nPixels );
		void	SetMinEntryHeight( Uint32 nPixels );
		void	SetMultiSelect( bool bMultiSelect );
		void	SetMultiSelectModifier( WgModifierKeys modifier );
		void	SetEntryBackground( const WgBlockSetPtr& pGfxEntryBg, bool bNormalBg = false,
									bool bDisabledBg = false, bool bMarkedBg = false );
		void	SetContextMenu( int button, Wdg_Menu * pMenu );

		WgTreeEntry *	AddEntry( long id, WgTreeEntry * pParent, WgTreeEntry * pBefore, WgItem * pItem );
//		WgTreeEntry *	AddJustInTimeEntry( Uint32 id, WgTreeEntry * pParent, WgTreeEntry * pBefore, WgSize sz );
//		TODO: Maybe support for JustInTimeContent as well? Usefull for display of file hierarchy...

		bool	MoveEntry( WgTreeEntry * pEntry, WgTreeEntry * pParent, WgTreeEntry * pBefore );

		bool	ReplaceEntry( WgTreeEntry * pEntry, WgItem * pNewItem );
		bool	DeleteEntry( WgTreeEntry * pEntry );
		bool	OpenEntry(  WgTreeEntry * pEntry );
		bool	CloseEntry(  WgTreeEntry * pEntry );
		bool	EnableEntry(  WgTreeEntry * pEntry );
		bool	DisableEntry(  WgTreeEntry * pEntry );
		bool	SelectEntry(  WgTreeEntry * pEntry );
		bool	UnselectEntry(  WgTreeEntry * pEntry );
		void	SelectAllEntries();
		void	UnselectAllEntries();
		void	DeleteAllEntries();
		void	OpenAllEntries();
		void	CloseAllEntries();
		void	EnableAllEntries();
		void	DisableAllEntries();
		void	GotoEntry( WgTreeEntry * pEntry );

		inline WgTreeEntry *	GetFirstEntry() { return m_entries.First(); }
		inline WgTreeEntry *	GetLastEntry() { return m_entries.Last(); }

		WgTreeEntry *	GetFirstSelectedEntry();
		WgTreeEntry *	GetLastSelectedEntry();

		WgTreeEntry *	GetNextSelectedEntry( WgTreeEntry * pEntry );
		WgTreeEntry *	GetPrevSelectedEntry( WgTreeEntry * pEntry );

		WgTreeEntry *	FindEntry( long id );

	protected:
		WgWidget * NewWidgetOfMyType() const;
	private:
		void	Init();
		void	DoMyOwnRender( const WgRect& _window, const WgRect& _clip, Uint8 _layer );
		void	DoMyOwnCloning( WgWidget * _pClone, const WgWidget * _pCloneRoot, const WgWidget * _pBranchRoot );
		bool	DoMyOwnMarkTest( int _x, int _y );
		void	DoMyOwnActionRespond( WgInput::UserAction action, int button_key, const WgActionDetails& info, const WgInput& inputObj );

//		void	DoMyOwnGeometryChangeSubclass( WgRect& oldGeo, WgRect& newGeo );

		WgSize			CalcSize( WgChain<WgTreeEntry>& chain );
		void			RefreshSize( WgTreeEntry * p );
		Uint32			GetEntryHeight( WgTreeEntry * pEntry );
		Uint32			GetEntryOfs( WgTreeEntry * pEntry );
		WgTreeEntry *	GetNextDisplayedEntry( WgTreeEntry * pEntry );
		WgTreeEntry *	FindEntryAtViewOfs( Sint32 ofs, Sint32 * wpOfsInside = 0 );
		WgRect			GetExpanderRect( WgTreeEntry * pEntry );

		Wdg_Menu *		m_pContextMenu[WG_MAX_BUTTONS];

		WgBlockSetPtr	m_pExpanderClosed;
		WgBlockSetPtr	m_pExpanderOpen;
		WgBlockSetPtr	m_pEntryBg;

		bool			m_entryBgFlags[WG_NB_MODES];

		bool			m_bNormalBg;			// Use background when normal?
		bool			m_bDisabledBg;			// Use background when disabled?
		bool			m_bMarkedBg;			// Use background when marked;

		WgSize			m_expanderSize;			// Size of expander graphics block (largest of their W/H respectively).
		Uint32			m_indentation;			// Indentation per level in pixels.
		Uint32			m_minEntryHeight;		// Minimum height of an entry.
		bool			m_bMultiSelect;
		bool			m_bDynamicItems;		// Set if items should be created/destroyed dynamically.
		WgModifierKeys	m_multiSelectModifier;	// Key-modifier used for multiselecting of entries.
/*
		Uint32			m_viewStartY;
		Uint32			m_viewStartSlot;
		Uint32			m_viewStartSlotPixelOfs;
*/
		WgChain<WgTreeEntry>	m_entries;
};

#endif //WDG_TREEVIEW_DOT_H
