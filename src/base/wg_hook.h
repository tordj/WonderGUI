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
	
	
	class RectLink;
	class RootPanel;
	
	class Receiver;
	typedef WgStrongPtr<Receiver,Object_p>	Receiver_p;
	typedef WgWeakPtr<Receiver,Object_wp>	Receiver_wp;
	
	
	class Widget;
	typedef WgStrongPtr<Widget,Receiver_p>	Widget_p;
	typedef WgWeakPtr<Widget,Receiver_wp>	Widget_wp;
	
	class Container;
	typedef	WgStrongPtr<Container,Widget_p>		Container_p;
	typedef	WgWeakPtr<Container,Widget_wp>		Container_wp;
	
	class RootPanel;
	typedef	WgStrongPtr<RootPanel,Object_p>		RootPanel_p;
	typedef	WgWeakPtr<RootPanel,Object_wp>		RootPanel_wp;
	
	class MsgRouter;
	typedef	WgStrongPtr<MsgRouter,Object_p>		MsgRouter_p;
	typedef	WgWeakPtr<MsgRouter,Object_wp>	MsgRouter_wp;
	
	class IWidgets;
	typedef	WgIStrongPtr<IWidgets,Interface_p>		IWidgets_p;
	typedef	WgIWeakPtr<IWidgets,Interface_wp>	IWidgets_wp;
	
	
	class Hook
	{
		friend class Widget;
		friend class Layer;
		friend class Panel;
		friend class Container;
		friend class VectorPanel;
		friend class Hook_p;
		friend class ModalLayer;
	
	public:
		virtual bool			isInstanceOf( const char * pClassName ) const;
		virtual const char *	className( void ) const;
		static const char		CLASSNAME[];
		static Hook_p		cast( const Hook_p& pInterface );				// Provided just for completeness sake.
	
		virtual Coord			pos() const = 0;							///< Get the local position of the widget.
		virtual Size			size() const = 0;							///< Get the size of the widget.
		virtual Rect			geo() const = 0;							///< Get the local geometry of the widget.
		virtual Coord			globalPos() const = 0;
		virtual Rect			globalGeo() const = 0;
	
		Hook_p			prev() const { return _prevHook(); }
		Hook_p			next() const { return _nextHook(); }
	
		Widget_p			widget() const;
		Container_p 		parent() const;
	
		RootPanel_p		root() const;
		MsgRouter_p	msgRouter() const;
	
	protected:
	
		Hook() : m_pWidget(0), m_pPtrHub(0) {}
		virtual ~Hook();
	
		virtual void	_setWidget( Widget * pWidget );				// Attach/release widget.
		void			_relinkWidget();								// Call when hook has been relocated, so old hook-pointer in widget is dirty and needs to be reset.
	
		Widget *		_widget() const { return m_pWidget; }
	
		// To be called by Widget
	
		virtual void	_requestRender() = 0;
		virtual void	_requestRender( const Rect& rect ) = 0;
		virtual void	_requestResize() = 0;
	
		virtual bool	_requestFocus();
		virtual bool	_releaseFocus();
	
		virtual Rect	_windowSection() const { return geo(); }			// Returns the window section within the canvas.
	
		//
	
		virtual Hook *	_prevHook() const = 0;
		virtual Hook *	_nextHook() const = 0;
		virtual Container * _parent() const = 0;
		virtual RootPanel * _root() const;
		virtual bool		_isVisible() const;
	
		Widget *		m_pWidget;
		Hook_pHub *	m_pPtrHub;
	};
	

} // namespace wg
#endif //WG_HOOK_DOT_H
