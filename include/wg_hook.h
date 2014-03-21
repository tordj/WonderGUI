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

#ifndef WG_HOOK_DOT_H
#define WG_HOOK_DOT_H

#ifndef	WG_GEO_DOT_H
#	include <wg_geo.h>
#endif

#ifndef WG_SMARTPTR_DOT_H
#	include <wg_smartptr.h>
#endif

#ifndef WG_HOOKPTR_DOT_H
#	include <wg_hookptr.h>
#endif


class WgContainer;
class WgGfxDevice;
class WgWidget;
class WgRectLink;
class WgRootPanel;


class WgWidget;
typedef WgSmartPtr<WgWidget,WgObjectPtr>			WgWidgetPtr;
typedef WgWeakPtr<WgWidget,WgObjectWeakPtr>			WgWidgetWeakPtr;

class WgContainer;
typedef	WgSmartPtr<WgContainer,WgWidgetPtr>			WgContainerPtr;
typedef	WgWeakPtr<WgContainer,WgWidgetWeakPtr>		WgContainerWeakPtr;

class WgRootPanel;
typedef	WgSmartPtr<WgRootPanel,WgObjectPtr>			WgRootPanelPtr;
typedef	WgWeakPtr<WgRootPanel,WgObjectWeakPtr>		WgRootPanelWeakPtr;

class WgEventHandler;
typedef	WgSmartPtr<WgEventHandler,WgObjectPtr>		WgEventHandlerPtr;
typedef	WgWeakPtr<WgEventHandler,WgObjectWeakPtr>	WgEventHandlerWeakPtr;

class WgIWidgetHolder;
typedef	WgISmartPtr<WgIWidgetHolder,WgInterfacePtr>		WgIWidgetHolderPtr;
typedef	WgWeakPtr<WgIWidgetHolder,WgInterfacePtr>		WgIWidgetHolderWeakPtr;


class	WgHook;



class WgHook
{
	friend class WgWidget;
	friend class WgLayer;
	friend class WgPanel;
	friend class WgContainer;
	friend class WgVectorPanel;
	friend class WgIWidgetHolder;
	friend class WgHookPtr;
	friend class WgModalLayer;
	friend class WgPopupLayer;

public:
	virtual bool			IsInstanceOf( const char * pClassName ) const;
	virtual const char *	ClassName( void ) const;
	static const char		CLASSNAME[];
	static WgHookPtr		Cast( const WgHookPtr& pInterface );				// Provided just for completeness sake.

	virtual WgCoord			Pos() const = 0;
	virtual WgSize			Size() const = 0;
	virtual WgRect			Geo() const = 0;
	virtual WgCoord			ScreenPos() const = 0;
	virtual WgRect			ScreenGeo() const = 0;

	WgHookPtr			Prev() const { return _prevHook(); }
	WgHookPtr			Next() const { return _nextHook(); }

	WgWidgetPtr			Widget() const;
	WgContainerPtr 		Parent() const;

	WgRootPanelPtr		Root() const;
	WgEventHandlerPtr	EventHandler() const;

protected:

	WgHook() : m_pWidget(0), m_pPtrHub(0) {}
	virtual ~WgHook();

	virtual void	_setWidget( WgWidget * pWidget );				// Attach/release widget.
	void			_relinkWidget();								// Call when hook has been relocated, so old hook-pointer in widget is dirty and needs to be reset.

	WgWidget *		_widget() const { return m_pWidget; }

	// To be called by Widget

	virtual void	_requestRender() = 0;
	virtual void	_requestRender( const WgRect& rect ) = 0;
	virtual void	_requestResize() = 0;

	virtual bool	_requestFocus();
	virtual bool	_releaseFocus();

	//

	virtual WgHook *	_prevHook() const = 0;
	virtual WgHook *	_nextHook() const = 0;
	virtual WgContainer * _parent() const = 0;
	virtual WgRootPanel * _root() const;

	WgWidget *		m_pWidget;
	WgHookPtrHub *	m_pPtrHub;
};
/*
template<typename T> T* WgCast(WgHook * pHook)
{
	if(pHook)
	{
		if(T::ClassType() == pHook->Type())
			return static_cast<T*>(pHook);
	}
	return 0;
}
*/
#endif //WG_HOOK_DOT_H
