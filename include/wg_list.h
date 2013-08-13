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

class WgList;
typedef	WgSmartPtr<WgList,WgContainerPtr>		WgListPtr;
typedef	WgWeakPtr<WgList,WgContainerWeakPtr>	WgListWeakPtr;

class WgListHook : public WgHook
{
public:
	WgListHook *	Prev() const { return static_cast<WgListHook*>(_prevHook()); }
	WgListHook *	Next() const { return static_cast<WgListHook*>(_nextHook()); }
	WgListPtr		Parent() const;

	virtual bool	SetVisible( bool bVisible );
	bool			IsVisible() { return m_bVisible; }

	virtual bool	SetSelected( bool bSelected );
	bool			IsSelected() { return m_bSelected; }


protected:

	WgListHook() : m_bVisible(true) {}
	 virtual ~WgListHook() {};

	 WgWidgetHolder* _holder() const { return _parent(); }


	 bool			m_bVisible;
	 bool			m_bSelected;
};

//____ WgList _________________________________________________________________

class WgList : public WgContainer
{
public:
	bool		IsInstanceOf( const char * pClassName ) const;
	const char *ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgListPtr	Cast( const WgObjectPtr& pObject );

	virtual void	SetSkin( const WgSkinPtr& pSkin );
	WgSkinPtr		Skin() const { return m_pSkin; }

	virtual void	SetEntrySkin( const WgSkinPtr& pSkin );
	virtual void	SetEntrySkins( const WgSkinPtr& pOddEntrySkin, const WgSkinPtr& pEvenEntrySkin );
	WgSkinPtr		OddEntrySkin() const { return m_pEntrySkin[0]; }
	WgSkinPtr		EvenEntrySkin() const { return m_pEntrySkin[1]; }

	void			SetSelectMode( WgSelectMode mode ) const;
	WgSelectMode	SelectMode() const { return m_selectMode; }

	inline WgListHook *	FirstHook() const { return static_cast<WgListHook*>(_firstHook()); }
	inline WgListHook *	LastHook() const { return static_cast<WgListHook*>(_lastHook()); }


protected:

	WgList();
	virtual ~WgList();

	WgSelectMode	m_selectMode;

	WgSkinPtr		m_pSkin;
	WgSkinPtr		m_pEntrySkin[2];
};



#endif //WG_LIST_DOT_H
