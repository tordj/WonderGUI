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

namespace wg 
{
	
	class IconField;
	class TextField;
	class List;
	typedef	StrongPtr<List,Container_p>		List_p;
	typedef	WeakPtr<List,Container_wp>	List_wp;
	
	class ListHook;
	typedef	HookTypePtr<ListHook,Hook_p>	ListHook_p;
	
	class ListHook : public Hook
	{
		friend class List;
	public:
		virtual bool			isInstanceOf( const char * pClassName ) const;
		virtual const char *	className( void ) const;
		static const char		CLASSNAME[];
		static ListHook_p	cast( const Hook_p& pInterface );
	
		ListHook_p	prev() const { return static_cast<ListHook*>(_prevHook()); }
		ListHook_p	next() const { return static_cast<ListHook*>(_nextHook()); }
		List_p		parent() const;
	
		virtual bool	setVisible( bool bVisible );
		bool			isVisible() { return m_bVisible; }
	
		virtual bool	setSelected( bool bSelected );
		bool			isSelected() { return m_pWidget->state().isSelected(); }
	
	
	protected:
	
		ListHook() : m_bVisible(true) {}
		 virtual ~ListHook() {};
	
	
		 bool			m_bVisible;
	};
	
	//____ List _________________________________________________________________
	
	class List : public Container
	{
		friend class ListHook;
	public:
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static List_p	cast( const Object_p& pObject );
	
		virtual void		setEntrySkin( const Skin_p& pSkin );
		virtual bool		setEntrySkin( const Skin_p& pOddEntrySkin, const Skin_p& pEvenEntrySkin );
		Skin_p			oddEntrySkin() const { return m_pEntrySkin[0]; }
		Skin_p			evenEntrySkin() const { return m_pEntrySkin[1]; }
	
		virtual void		setLassoSkin( const Skin_p& pSkin );
		Skin_p			lassoSkin() const { return m_pLassoSkin; }
	
		virtual bool		setSelectMode( SelectMode mode );
		SelectMode		selectMode() const { return m_selectMode; }
	
	
	
		inline ListHook_p	firstHook() const { return static_cast<ListHook*>(_firstHook()); }
		inline ListHook_p	lastHook() const { return static_cast<ListHook*>(_lastHook()); }
	
	protected:
	
		List();
		virtual ~List();
	
		virtual void	_onMsg( const Msg_p& pMsg );
	
		virtual bool	_selectEntry( ListHook * pHook, bool bSelected, bool bPostMsg );
		virtual int		_selectRange( ListHook * pFirst, ListHook * pLast, bool bSelected, bool bPostMsg );
		virtual int		_flipRange( ListHook * pFirst, ListHook * pLast, bool bPostMsg );
		virtual void	_clearSelected( bool bPostMsg );
		virtual void	_onWidgetAppeared( ListHook * pInserted ) = 0;
		virtual void	_onWidgetDisappeared( ListHook * pToBeRemoved ) = 0;		// Call BEFORE widget is removed from m_hooks.
		virtual ListHook * _findEntry( const Coord& ofs ) = 0;
		virtual void	_getEntryGeo( Rect& geo, const ListHook * pHook ) const = 0;
	
		virtual Rect	_listArea() const = 0;										// Area for the entries (contentRect minus header).
		virtual Rect	_listWindow() const = 0;
		virtual Rect	_listCanvas() const = 0;
	
		virtual void	_onEntrySkinChanged( Size oldPadding, Size newPadding ) = 0;
		virtual void	_onLassoUpdated( const Rect& oldLasso, const Rect& newLasso ) = 0;
		void			_renderHeader( GfxDevice * pDevice, const Rect& _window, const Rect& _clip, 
										const Skin_p& pSkin, TextField * pText, IconField * pLabelIcon, 
										IconField * pSortIcon, State state, bool bShowSortIcon, bool bInvertedSort );
	
		SelectMode	m_selectMode;
		Skin_p		m_pEntrySkin[2];
		Skin_p		m_pLassoSkin;
		bool			m_bOpaqueEntries;
	
		Coord			m_lassoBegin;
		Coord			m_lassoEnd;
	
		ListHook_p	m_pFocusedEntry;
		ListHook_p	m_pHoveredEntry;
	};
	
	
	

} // namespace wg
#endif //WG_LIST_DOT_H
