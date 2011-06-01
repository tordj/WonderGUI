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

#ifndef WG_VBOXLAYOUT_DOT_H
#define WG_VBOXLAYOUT_DOT_H

class WgVBoxHook : public WgOrdSelHook
{
public:
	inline WgVBoxHook * PrevHook() const { return Prev(); }
	inline WgVBoxHook * NextHook() const { return Next(); }

	inline WgVBoxHook * PrevSelectedHook() const { return _prevSelectedHook(); }
	inline WgVBoxHook * NextSelectedHook() const { return _nextSelectedHook(); }

	inline WgVBoxLayout * Parent() const { return m_pParent; }

protected:
	PROTECTED_LINK_METHODS( WgVBoxHook );

	WgVBoxHook( WgGizmo * pGizmo, WgVBoxLayout * pParent );

	WgGizmoContainer* _parent() const;

	WgVBoxLayout *	m_pParent;
};

class WgVBoxLayout : public WgOrdSelLayout
{
public:
	WgVBoxLayout();
	virtual ~WgVBoxLayout();

	virtual const char * Type() const;
	static const char * GetMyType();

	inline WgVBoxHook * AddGizmo( WgGizmo * pGizmo ) { WgOrderedLayout::AddGizmo(pGizmo); }
	inline WgVBoxHook * InsertGizmo( WgGizmo * pGizmo, WgGizmo * pSibling ) { WgOrderedLayout::InsertGizmo(pGizmo,pSibling); }
	inline WgVBoxHook * InsertGizmoSorted( WgGizmo * pGizmo ) { WgOrderedLayout::InsertGizmoSorted(pGizmo); }

	inline WgVBoxHook* FirstHook() const { return static_cast<WgVBoxHook*>(m_hooks.First()); }
	inline WgVBoxHook* LastHook() const { return static_cast<WgVBoxHook*>(m_hooks.Last()); }

	inline WgVBoxHook* FirstSelectedHook() const { return static_cast<WgVBoxHook*>(_firstSelectedHook()); }
	inline WgVBoxHook* LastSelectedHook() const { return static_cast<WgVBoxHook*>(_lastSelectedHook()); }


	// Overloaded from WgGizmo

	int		HeightForWidth( int width ) const;
	int		WidthForHeight( int height ) const;

	WgSize	MinSize() const;
	WgSize	BestSize() const;
	WgSize	MaxSize() const;

protected:

//	void	_onCloneContent( const WgGizmo * _pOrg );
	void	_onNewSize( const WgSize& size );
	void	_onAction( WgInput::UserAction action, int button_key, const WgActionDetails& info, const WgInput& inputObj );


};



#endif //WG_VBOXLAYOUT_DOT_H