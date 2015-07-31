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
	
	class WgIconField;
	class WgTextField;
	class WgList;
	typedef	WgStrongPtr<WgList,WgContainer_p>		WgList_p;
	typedef	WgWeakPtr<WgList,WgContainer_wp>	WgList_wp;
	
	class WgListHook;
	typedef	WgHookTypePtr<WgListHook,WgHook_p>	WgListHook_p;
	
	class WgListHook : public WgHook
	{
		friend class WgList;
	public:
		virtual bool			isInstanceOf( const char * pClassName ) const;
		virtual const char *	className( void ) const;
		static const char		CLASSNAME[];
		static WgListHook_p	cast( const WgHook_p& pInterface );
	
		WgListHook_p	prev() const { return static_cast<WgListHook*>(_prevHook()); }
		WgListHook_p	next() const { return static_cast<WgListHook*>(_nextHook()); }
		WgList_p		parent() const;
	
		virtual bool	setVisible( bool bVisible );
		bool			isVisible() { return m_bVisible; }
	
		virtual bool	setSelected( bool bSelected );
		bool			isSelected() { return m_pWidget->state().isSelected(); }
	
	
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
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static WgList_p	cast( const WgObject_p& pObject );
	
		virtual void		setEntrySkin( const WgSkin_p& pSkin );
		virtual bool		setEntrySkin( const WgSkin_p& pOddEntrySkin, const WgSkin_p& pEvenEntrySkin );
		WgSkin_p			oddEntrySkin() const { return m_pEntrySkin[0]; }
		WgSkin_p			evenEntrySkin() const { return m_pEntrySkin[1]; }
	
		virtual void		setLassoSkin( const WgSkin_p& pSkin );
		WgSkin_p			lassoSkin() const { return m_pLassoSkin; }
	
		virtual bool		setSelectMode( WgSelectMode mode );
		WgSelectMode		selectMode() const { return m_selectMode; }
	
	
	
		inline WgListHook_p	firstHook() const { return static_cast<WgListHook*>(_firstHook()); }
		inline WgListHook_p	lastHook() const { return static_cast<WgListHook*>(_lastHook()); }
	
	protected:
	
		WgList();
		virtual ~WgList();
	
		virtual void	_onMsg( const WgMsg_p& pMsg );
	
		virtual bool	_selectEntry( WgListHook * pHook, bool bSelected, bool bPostMsg );
		virtual int		_selectRange( WgListHook * pFirst, WgListHook * pLast, bool bSelected, bool bPostMsg );
		virtual int		_flipRange( WgListHook * pFirst, WgListHook * pLast, bool bPostMsg );
		virtual void	_clearSelected( bool bPostMsg );
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
										const WgSkin_p& pSkin, WgTextField * pText, WgIconField * pLabelIcon, 
										WgIconField * pSortIcon, WgState state, bool bShowSortIcon, bool bInvertedSort );
	
		WgSelectMode	m_selectMode;
		WgSkin_p		m_pEntrySkin[2];
		WgSkin_p		m_pLassoSkin;
		bool			m_bOpaqueEntries;
	
		WgCoord			m_lassoBegin;
		WgCoord			m_lassoEnd;
	
		WgListHook_p	m_pFocusedEntry;
		WgListHook_p	m_pHoveredEntry;
	};
	
	
	

} // namespace wg
#endif //WG_LIST_DOT_H
