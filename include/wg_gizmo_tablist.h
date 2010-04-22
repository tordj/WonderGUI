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

#ifndef	WG_GIZMO_TABLIST_DOT_H
#define	WG_GIZMO_TABLIST_DOT_H

#ifndef	WG_GIZMO_DOT_H
#	include <wg_gizmo.h>
#endif

#ifndef WG_CHAIN_DOT_H
#	include <wg_chain.h>
#endif

#ifndef WG_TEXT_DOT_H
#	include <wg_text.h>
#endif

#ifndef WG_BLOCKSET_DOT_H
#	include <wg_blockset.h>
#endif

class WgFont;
class WgSurface;

class WgItemRow;

class WgTab : public WgLink
{
	friend class WgGizmoTablist;
public:
	WgTab( Sint32 _id );
	virtual ~WgTab();

	LINK_METHODS( WgTab );

	Sint32			GetId() const			{ return m_id; }
	void			SetAlert(bool bAlertOn) { m_bAlert = bAlertOn; }
	bool			GetAlert() const		{ return m_bAlert; }

	const WgChar*	getText() const				{ return m_text.getText(); }
	void			setText(const char* pText)	{ return m_text.setText(pText); }

	void			SetSource( const WgBlockSetPtr& pGfx ) { m_pGfx = pGfx; }
	WgBlockSetPtr	GetSource() const			{ return m_pGfx; }

private:

	WgText			m_text;
	Uint32			m_width;
	Sint32			m_id;
	bool			m_bAlert;
	WgBlockSetPtr	m_pGfx;

	WgItemRow*		m_pItemRow;
	bool			m_bLockedContent;
	bool			m_bVisible;
};

class WgGizmoTablist : public WgGizmo
{
public:
	// It's possible to specify different sources for leftmost, rightmost, and middle tabs
	enum SourceType
	{
		SourceTypeLeft,
		SourceTypeMiddle,
		SourceTypeRight,
        SourceTypeAll,
	};
	static const int m_nSourceTypes = 3;

	enum TabWidthMode
	{
		TabWidthModeNormal,	// All tabs have required width
		TabWidthModeUnified,// All tabs have same width
		TabWidthModeExpand,	// Tabs are stretched out to cover the entire width of the tab list
	};

	WgGizmoTablist();
	~WgGizmoTablist();
	virtual const char * Type() const;
	static const char * GetMyType();

	//____ Methods __________________________________________

	void			SetTextProperties( const WgTextPropPtr& pProp );
	WgTextPropPtr	GetTextProperties() const { return m_pProp; }

	void			SetSource( WgBlockSetPtr pBlockSet, SourceType type = SourceTypeAll);
	WgBlockSetPtr	GetSource(SourceType type = SourceTypeMiddle) const			{ return m_sources[type]; }

	void	SetOverlap( Uint16 overlap );
	Uint16	GetOverlap() const { return m_overlap; }

	void 	SetMinTabWidth( Uint16 minWidth );
	Uint16 	GetMinTabWidth() const { return m_minTabWidth; }

	void 	SetAlertRate( Uint32 millisec );
	Uint32	GetAlertRate() const { return m_alertRate; }

	void 	SetTextOrigo( WgOrigo origo );
	WgOrigo	GetTextOrigo() const { return m_textOrigo; }

	void 			SetTabWidthMode(TabWidthMode mode);
	TabWidthMode	GetTabWidthMode() const { return m_tabWidthMode; }

	bool AddTab( Sint32 id, const char * pText, Uint32 position = 0xFFFFFFFF, const WgBlockSetPtr& pGfx = 0 );
	bool AddTab( Sint32 id, const Uint16* pText, Uint32 position = 0xFFFFFFFF, const WgBlockSetPtr& pGfx = 0 );
	bool AddTab( Sint32 id, const WgChar* pText, Uint32 position = 0xFFFFFFFF, const WgBlockSetPtr& pGfx = 0 );
	bool AddTab( Sint32 id, const WgText * pText, Uint32 position = 0xFFFFFFFF, const WgBlockSetPtr& pGfx = 0 );
	bool RemoveTab( Sint32 id );
	void RemoveAllTabs( );

	bool MoveTab( Sint32 id, Uint32 position );
	bool SetTabText( Sint32 id, const char * pText );
	bool SetTabText( Sint32 id, const Uint16* pText );
	bool SetTabText( Sint32 id, const WgText * pText );

	bool SetTabId( Sint32 id, Sint32 newId );
	Sint32 GetTabId(Uint32 position);

	bool SetTabTextColor( Sint32 id, WgColor col );

	Uint32	GetTabWidth( Sint32 id );

	bool SelectTab( Sint32 id );
	int GetSelectedTabId() const;
	int GetSelectedTabPos() const;
	bool SetAlert( Sint32 id, bool bAlertOn );
	bool GetAlert( Sint32 id );
	bool ShowTab( Sint32 id, bool bVisible );
	Uint32 GetTabCount( ) const;

	bool HasTab( Sint32 id );

	WgItemRow*	LockTabContent( Sint32 id );
	void		UnlockTabContent( Sint32 id );

	WgTab*			GetFirstTab();
	WgTab*			GetLastTab();
	WgTab* 			GetFirstVisibleTab();
	WgTab* 			GetLastVisibleTab();


protected:
	void	OnCloneContent( const WgGizmo * _pOrg );
	void	OnRender( WgGfxDevice * pDevice, const WgRect& _window, const WgRect& _clip, Uint8 _layer );
	void	OnRefresh();
	void	OnUpdate( const WgUpdateInfo& _updateInfo );
	void	OnAction( WgEmitter * pEmitter, WgInput::UserAction action, int button_key, const WgActionDetails& info, const WgInput& inputObj );
	bool	OnMarkTest( const WgCord& ofs );

private:

	WgBlockSetPtr	GetTabSource( WgTab * pTab );
	WgMode			GetTabMode(const WgTab& tab);
	WgTab*			FindTab( Sint32 id );
	bool			ResizeTab( WgTab * pTab );
	bool			ResizeAllTabs();
	WgTab*	 		Pos2Tab( Sint32 x, Sint32 y );
	float			CalcTabScaleFactor();

	void	RenderTab( WgGfxDevice * pDevice, WgTab& tab, WgRect dest, const WgRect& clip );

	WgChain<WgTab>	m_tabs;

	WgTextPropPtr	m_pProp;

	WgOrigo			m_textOrigo;
	WgBlockSetPtr	m_sources[m_nSourceTypes];

	WgTab *		m_pTabSelected;
	WgTab *		m_pTabMarked;

	bool		m_bAlertOn;
	Sint16		m_alertModeCnt;
	Uint16		m_alertRate;				// Milliseconds between switching graphics.

	Uint16		m_minTabWidth;
	Uint16		m_overlap;

	TabWidthMode m_tabWidthMode;
	Uint16		m_widestTab;
};





#endif //WG_GIZMO_TABLIST_DOT_H
