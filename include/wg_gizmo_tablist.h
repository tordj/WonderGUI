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
class WgCharSeq;
class WgItemRow;

class WgTab : public WgLink
{
	friend class WgGizmoTablist;
public:
	WgTab( int id );
	virtual ~WgTab();

	LINK_METHODS( WgTab );

	int				GetId() const			{ return m_id; }
	void			SetAlert(bool bAlertOn) { m_bAlert = bAlertOn; }
	bool			GetAlert() const		{ return m_bAlert; }

	const WgChar*	getText() const				{ return m_text.getText(); }
	void			setText(const char* pText)	{ return m_text.setText(pText); }

	void			SetSource( const WgBlockSetPtr& pGfx ) { m_pGfx = pGfx; }
	WgBlockSetPtr	GetSource() const			{ return m_pGfx; }

	void			SetId( int id ) { m_id = id; }

private:

	WgText			m_text;
	Uint32			m_width;
	int				m_id;
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
		TabWidthModeExpand2,// Tabs are overlapped to cover the entire width of the tab list
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

	bool AddTab( int id, const WgCharSeq& text, int position = -1, const WgBlockSetPtr& pGfx = 0 );
	bool RemoveTab( int id );
	void RemoveAllTabs( );

	bool MoveTab( int id, int position );
	bool SetTabText( int id, const WgCharSeq& text );

	bool SetTabId( int id, int newId );
	int GetTabId(int position);

	void	setTabMouseOpacity( bool bOpaque ) { m_bTabOpaqueForMouse = bOpaque; };
	bool	getTabMouseOpacity() const { return m_bTabOpaqueForMouse; }


	bool SetTabTextColor( int id, WgColor col );

	Uint32	GetTabWidth( int id );

	bool SelectTab( int id );
	int GetSelectedTabId() const;
	int GetSelectedTabPos() const;
	bool SetAlert( int id, bool bAlertOn );
	bool GetAlert( int id );
	bool ShowTab( int id, bool bVisible );
	int GetTabCount( ) const;

	bool HasTab( int id );

	WgItemRow*	LockTabContent( int id );
	void		UnlockTabContent( int id );

	WgTab*			GetSelectedTab();
	WgTab*			GetFirstTab();
	WgTab*			GetLastTab();
	WgTab* 			GetFirstVisibleTab();
	WgTab* 			GetLastVisibleTab();


protected:
	void	OnCloneContent( const WgGizmo * _pOrg );
	void	OnRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip, Uint8 _layer );
	void	OnRefresh();
	void	OnUpdate( const WgUpdateInfo& _updateInfo );
	void	OnAction( WgInput::UserAction action, int button_key, const WgActionDetails& info, const WgInput& inputObj );
	bool	OnAlphaTest( const WgCord& ofs );

private:

	WgBlockSetPtr	GetTabSource( WgTab * pTab );
	WgMode			GetTabMode(const WgTab& tab);
	WgTab*			FindTab( int id );
	bool			ResizeTab( WgTab * pTab );
	bool			ResizeAllTabs();
	WgTab*	 		Pos2Tab( int x, int y );
	float			CalcTabScaleFactor();

	void	RenderTab( WgGfxDevice * pDevice, WgTab& tab, WgRect dest, const WgRect& clip );

	WgChain<WgTab>	m_tabs;

	WgTextPropPtr	m_pProp;

	WgOrigo			m_textOrigo;
	WgBlockSetPtr	m_sources[m_nSourceTypes];

	WgTab *		m_pTabSelected;
	WgTab *		m_pTabMarked;

	bool		m_bTabOpaqueForMouse;
	bool		m_bAlertOn;
	Sint16		m_alertModeCnt;
	Uint16		m_alertRate;				// Milliseconds between switching graphics.

	Uint16		m_minTabWidth;
	Sint16		m_overlap;

	TabWidthMode m_tabWidthMode;
	Uint16		m_widestTab;
};





#endif //WG_GIZMO_TABLIST_DOT_H
