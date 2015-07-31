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

#ifndef WG_POINTERS_DOT_H
#	include <wg_pointers.h>
#endif

#ifndef WG_HOOKPTR_DOT_H
#	include <wg_hookptr.h>
#endif

namespace wg 
{
	
	
	class WgRectLink;
	class WgRootPanel;
	
	class WgReceiver;
	typedef WgStrongPtr<WgReceiver,WgObject_p>	WgReceiver_p;
	typedef WgWeakPtr<WgReceiver,WgObject_wp>	WgReceiver_wp;
	
	
	class WgWidget;
	typedef WgStrongPtr<WgWidget,WgReceiver_p>	WgWidget_p;
	typedef WgWeakPtr<WgWidget,WgReceiver_wp>	WgWidget_wp;
	
	class WgContainer;
	typedef	WgStrongPtr<WgContainer,WgWidget_p>		WgContainer_p;
	typedef	WgWeakPtr<WgContainer,WgWidget_wp>		WgContainer_wp;
	
	class WgRootPanel;
	typedef	WgStrongPtr<WgRootPanel,WgObject_p>		WgRootPanel_p;
	typedef	WgWeakPtr<WgRootPanel,WgObject_wp>		WgRootPanel_wp;
	
	class WgMsgRouter;
	typedef	WgStrongPtr<WgMsgRouter,WgObject_p>		WgMsgRouter_p;
	typedef	WgWeakPtr<WgMsgRouter,WgObject_wp>	WgMsgRouter_wp;
	
	class WgIWidgets;
	typedef	WgIStrongPtr<WgIWidgets,WgInterface_p>		WgIWidgets_p;
	typedef	WgIWeakPtr<WgIWidgets,WgInterface_wp>	WgIWidgets_wp;
	
	
	class WgHook
	{
		friend class WgWidget;
		friend class WgLayer;
		friend class WgPanel;
		friend class WgContainer;
		friend class WgVectorPanel;
		friend class WgHook_p;
		friend class WgModalLayer;
	
	public:
		virtual bool			isInstanceOf( const char * pClassName ) const;
		virtual const char *	className( void ) const;
		static const char		CLASSNAME[];
		static WgHook_p		cast( const WgHook_p& pInterface );				// Provided just for completeness sake.
	
		virtual WgCoord			pos() const = 0;							///< Get the local position of the widget.
		virtual WgSize			size() const = 0;							///< Get the size of the widget.
		virtual WgRect			geo() const = 0;							///< Get the local geometry of the widget.
		virtual WgCoord			globalPos() const = 0;
		virtual WgRect			globalGeo() const = 0;
	
		WgHook_p			prev() const { return _prevHook(); }
		WgHook_p			next() const { return _nextHook(); }
	
		WgWidget_p			widget() const;
		WgContainer_p 		parent() const;
	
		WgRootPanel_p		root() const;
		WgMsgRouter_p	msgRouter() const;
	
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
	
		virtual WgRect	_windowSection() const { return geo(); }			// Returns the window section within the canvas.
	
		//
	
		virtual WgHook *	_prevHook() const = 0;
		virtual WgHook *	_nextHook() const = 0;
		virtual WgContainer * _parent() const = 0;
		virtual WgRootPanel * _root() const;
		virtual bool		_isVisible() const;
	
		WgWidget *		m_pWidget;
		WgHook_pHub *	m_pPtrHub;
	};
	

} // namespace wg
#endif //WG_HOOK_DOT_H
