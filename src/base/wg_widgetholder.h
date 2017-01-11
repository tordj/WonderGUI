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


#ifndef WG_WIDGETHOLDER_DOT_H
#define WG_WIDGETHOLDER_DOT_H

namespace wg 
{

	class WidgetHolder
	{
	public:
		virtual Coord			_widgetPos( void * pChildRef ) const;			///< Get the local position of the widget.
		virtual Size			_widgetSize( void * pChildRef ) const;						///< Get the size of the widget.
		virtual Rect			_widgetGeo( void * pChildRef ) const;						///< Get the local geometry of the widget.
		virtual Coord			_widgetGlobalPos( void * pChildRef ) const;
		virtual Rect			_widgetGlobalGeo( void * pChildRef ) const;
	
	
		virtual Container *  	_parent() const;
		virtual RootPanel *		_root() const;



		virtual void	_widgetRenderRequested( void * pChildRef );
		virtual void	_widgetRenderRequested( void * pChildRef, const Rect& rect );
		virtual void	_widgetResizeRequested( void * pChildRef );
	
		inline  bool	_requestFocus() { return _requestFocus(m_pWidget); }
		virtual bool	_requestFocus( Widget * pWidget );					// Request focus on behalf of child/grandchild.
		inline  bool	_releaseFocus() { return _releaseFocus(m_pWidget); }
		virtual bool	_releaseFocus( Widget * pWidget );
	
		virtual void	_requestVisibility();
		virtual void	_requestVisibility( const Rect& preferred, const Rect& prio );
	
		virtual Rect	_windowSection() const { return Rect(0,0,size()); }			// Returns the window section within the canvas.
	
	
	
		//
	
		virtual Hook *	_prevHook() const = 0;
		virtual Hook *	_nextHook() const = 0;
		virtual bool		_isVisible() const;
		
	};
	
	
}


#endif //WG_WIDGETHOLDER_DOT_H