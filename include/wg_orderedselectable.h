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

#ifndef WG_ORDERED_SELECTABLE_DOT_H
#define WG_ORDERED_SELECTABLE_DOT_H

class WgOrdSelHook : public WgOrderedHook
{
public:
	void			SetSelected( bool bSelected );
	bool			IsSelected() const { return m_bSelected; }

	inline WgOrdSelHook*	PrevHook() const { return Prev(); }
	inline WgOrdSelHook*	NextHook() const { return Next(); }
	inline WgOrdSelHook*	Parent() const { return (WgOrdSelLayout*) _parent(); }

protected:
	PROTECTED_LINK_METHODS( WgOrdSelHook );

	WgOrdSelHook( WgGizmo * pGizmo, WgGizmoOrdSelHook * pParent );
	~WgOrdSelHook();

	WgOrdSelHook *	_prevSelectedHook() const;
	WgOrdSelHook *	_nextSelectedHook() const;

	bool			m_bSelectable;
	bool			m_bSelected;
};

class WgOrdSelLayout : public WgOrderedLayout
{
public:
	int				SelectAll();
	void			ClearSelection();

	int				GetNbSelected();

	void			SetSelectMode( WgSelectMode mode );
	inline WgSelectMode	SelectMode() const					{ return m_selectMode; }

	inline void		SetScrollToSelected( bool bScroll )		{ m_bScrollOnSelect = bScroll; }
	inline bool		ScrollToSelect() const				{ return m_bScrollOnSelect; }

	WgGizmo *		FirstSelectedGizmo() const;
	WgGizmo *		LastSelectedGizmo() const;

	void			SetSelectionBg( const WgBlockSetPtr& blocks );
	void			SetSelectionBg( WgColor bgColor );
	


protected:
	void			_onCloneContent( const WgGizmo * _pOrg );


	WgOrdSelHook *	_firstSelectedHook() const;
	WgOrdSelHook *	_lastSelectedHook() const;

	WgColor			m_selectionBgColor;
	WgBlockSetPtr	m_pSelectionBg;
	WgBlockSetPtr	m_pSelectionFg;

	bool			m_bScrollOnSelect;
	WgSelectMode	m_selectMode;
	WgOrdSelHook *	m_pLatestSelected;				// Pointer at latest selected hook if any is selected.

};


#endif //WG_ORDERED_SELECTABLE_DOT_H