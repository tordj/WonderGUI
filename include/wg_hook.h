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

class WgWidgetHolder;
class WgContainer;
class WgGfxDevice;
class WgWidget;
class WgRectLink;
class WgRootPanel;


class WgWidget;
typedef class WgSmartPtr<WgWidget,WgObjectPtr> WgWidgetPtr;
typedef class WgWeakPtr<WgWidget,WgObjectWeakPtr> WgWidgetWeakPtr;

class WgContainer;
typedef	WgSmartPtr<WgContainer,WgWidgetPtr>		WgContainerPtr;
typedef	WgWeakPtr<WgContainer,WgWidgetWeakPtr>		WgContainerWeakPtr;

class WgRootPanel;
typedef	WgSmartPtr<WgRootPanel,WgObjectPtr>		WgRootPanelPtr;
typedef	WgWeakPtr<WgRootPanel,WgObjectWeakPtr>	WgRootPanelWeakPtr;

class WgEventHandler;
typedef	WgSmartPtr<WgEventHandler,WgObjectPtr>		WgEventHandlerPtr;
typedef	WgWeakPtr<WgEventHandler,WgObjectWeakPtr>	WgEventHandlerWeakPtr;


class WgHook
{
	friend class WgWidget;
	friend class WgLayer;
	friend class WgPanel;
	friend class WgContainer;
	friend class WgVectorPanel;
	friend class WgWidgetHolder;


public:
	virtual WgCoord	Pos() const = 0;
	virtual WgSize	Size() const = 0;
	virtual WgRect	Geo() const = 0;
	virtual WgCoord	ScreenPos() const = 0;
	virtual WgRect	ScreenGeo() const = 0;

	WgHook *		Prev() const { return _prevHook(); }
	WgHook *		Next() const { return _nextHook(); }

	WgWidgetPtr			Widget() const;
	WgWidgetHolder * 	Holder() const;
	WgContainerPtr 		Parent() const;

	WgRootPanelPtr		Root() const;
	WgEventHandlerPtr	EventHandler() const;

	virtual const char *Type( void ) const = 0;

	
protected:

	WgHook() : m_pWidget(0) {}
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
	virtual WgWidgetHolder * _holder() const = 0;
	virtual WgContainer * _parent() const = 0;
	virtual WgRootPanel * _root() const;

	WgWidget *		m_pWidget;
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
