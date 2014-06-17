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

#ifndef	WG_LIST_DOT_H
#define	WG_LIST_DOT_H

#ifndef WG_WIDGET_DOT_H
#	include <wg_widget.h>
#endif

#ifndef WG_CONTAINER_DOT_H
#	include <wg_container.h>
#endif

#ifndef WG_SKIN_DOT_H
#	include <wg_skin.h>
#endif

class WgIconField;

class WgList;
typedef	WgStrongPtr<WgList,WgContainerPtr>		WgListPtr;
typedef	WgWeakPtr<WgList,WgContainerWeakPtr>	WgListWeakPtr;

class WgListHook;
typedef	WgHookTypePtr<WgListHook,WgHookPtr>	WgListHookPtr;

class WgListHook : public WgHook
{
	friend class WgList;
public:
	virtual bool			IsInstanceOf( const char * pClassName ) const;
	virtual const char *	ClassName( void ) const;
	static const char		CLASSNAME[];
	static WgListHookPtr	Cast( const WgHookPtr& pInterface );

	WgListHookPtr	Prev() const { return static_cast<WgListHook*>(_prevHook()); }
	WgListHookPtr	Next() const { return static_cast<WgListHook*>(_nextHook()); }
	WgListPtr		Parent() const;

	virtual bool	SetVisible( bool bVisible );
	bool			IsVisible() { return m_bVisible; }

	virtual bool	SetSelected( bool bSelected );
	bool			IsSelected() { return m_pWidget->State().IsSelected(); }


protected:

	WgListHook() : m_bVisible(true) {}
	 virtual ~WgListHook() {};


	 bool			m_bVisible;
};

//____ WgList _________________________________________________________________

class WgList : public WgContainer
{
	friend class WgListHook;
public:
	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgListPtr	Cast( const WgObjectPtr& pObject );

	virtual void		SetEntrySkin( const WgSkinPtr& pSkin );
	virtual bool		SetEntrySkin( const WgSkinPtr& pOddEntrySkin, const WgSkinPtr& pEvenEntrySkin );
	WgSkinPtr			OddEntrySkin() const { return m_pEntrySkin[0]; }
	WgSkinPtr			EvenEntrySkin() const { return m_pEntrySkin[1]; }

	virtual void		SetLassoSkin( const WgSkinPtr& pSkin );
	WgSkinPtr			LassoSkin() const { return m_pLassoSkin; }

	virtual bool		SetSelectMode( WgSelectMode mode );
	WgSelectMode		SelectMode() const { return m_selectMode; }



	inline WgListHookPtr	FirstHook() const { return static_cast<WgListHook*>(_firstHook()); }
	inline WgListHookPtr	LastHook() const { return static_cast<WgListHook*>(_lastHook()); }

protected:

	WgList();
	virtual ~WgList();

	virtual void	_onEvent( const WgEventPtr& pEvent, WgEventHandler * pHandler );

	virtual bool	_selectEntry( WgListHook * pHook, bool bSelected, bool bPostEvent );
	virtual int		_selectRange( WgListHook * pFirst, WgListHook * pLast, bool bSelected, bool bPostEvent );
	virtual int		_flipRange( WgListHook * pFirst, WgListHook * pLast, bool bPostEvent );
	virtual void	_clearSelected( bool bPostEvent );
	virtual void	_onWidgetAppeared( WgListHook * pInserted ) = 0;
	virtual void	_onWidgetDisappeared( WgListHook * pToBeRemoved ) = 0;		// Call BEFORE widget is removed from m_hooks.
	virtual WgListHook * _findEntry( const WgCoord& ofs ) = 0;
	virtual void	_getEntryGeo( WgRect& geo, const WgListHook * pHook ) const = 0;

	virtual WgRect	_listArea() const = 0;										// Area for the entries (contentRect minus header).
	virtual WgRect	_listWindow() const = 0;
	virtual WgRect	_listCanvas() const = 0;

	virtual void	_onEntrySkinChanged( WgSize oldPadding, WgSize newPadding ) = 0;
	virtual void	_onLassoUpdated( const WgRect& oldLasso, const WgRect& newLasso ) = 0;
	void			_renderHeader( WgGfxDevice * pDevice, const WgRect& _window, const WgRect& _clip, 
									const WgSkinPtr& pSkin, WgTextField * pText, WgIconField * pLabelIcon, 
									WgIconField * pSortIcon, WgState state, bool bShowSortIcon, bool bInvertedSort );

	WgSelectMode	m_selectMode;
	WgSkinPtr		m_pEntrySkin[2];
	WgSkinPtr		m_pLassoSkin;
	bool			m_bOpaqueEntries;

	WgCoord			m_lassoBegin;
	WgCoord			m_lassoEnd;

	WgListHookPtr	m_pFocusedEntry;
	WgListHookPtr	m_pHoveredEntry;
};



#endif //WG_LIST_DOT_H
