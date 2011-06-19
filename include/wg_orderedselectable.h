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

#ifndef WG_ORDERED_LAYOUT_DOT_H
#	include <wg_orderedlayout.h>
#endif

#ifndef WG_BLOCKSET_DOT_H
#	include <wg_blockset.h>
#endif

class WgOrdSelLayout;


class WgOrdSelHook : public WgOrderedHook
{
public:
	void					SetSelected( bool bSelected );
	inline bool				IsSelected() const { return m_bSelected; }

	inline WgOrdSelHook*	PrevHook() const { return _prev(); }
	inline WgOrdSelHook*	NextHook() const { return _next(); }
	inline WgOrdSelLayout*	Parent() const { return (WgOrdSelLayout*) _parent(); }

protected:
	PROTECTED_LINK_METHODS( WgOrdSelHook );

	WgOrdSelHook( WgGizmo * pGizmo );
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


	void			SetChildBg( const WgBlockSetPtr& pOddBg, const WgBlockSetPtr& pEvenBg );
	void			SetChildFg( const WgBlockSetPtr& pFg );
//	void			SetChildBg( const WgColorSetPtr& pOddBg, const WgColorSetPtr& pEvenBg );


protected:
	void			_onCloneContent( const WgGizmo * _pOrg );

	WgOrdSelHook *	_firstSelectedHook() const;
	WgOrdSelHook *	_lastSelectedHook() const;

	WgColorSetPtr	m_pOddBgColor;
	WgColorSetPtr	m_pEvenBgColor;
	WgBlockSetPtr	m_pOddBgGfx;
	WgBlockSetPtr	m_pEvenBgGfx;
	WgBlockSetPtr	m_pFgGfx;

	bool			m_bScrollOnSelect;
	WgSelectMode	m_selectMode;
	WgOrdSelHook *	m_pLatestSelected;				// Pointer at latest selected hook if any is selected.

};


#endif //WG_ORDERED_SELECTABLE_DOT_H
