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

#ifndef	WG_CONTAINER_DOT_H
#define	WG_CONTAINER_DOT_H

#ifndef WG_WIDGET_DOT_H
#	include <wg_widget.h>
#endif

namespace wg 
{
	
	class Patches;
	class ModalLayer;
	class PopupLayer;
	
	class Container;
	typedef	WgStrongPtr<Container,Widget_p>			Container_p;
	typedef	WgWeakPtr<Container,Widget_wp>		Container_wp;
	
	/**
	 * @brief Base class for all widgets that can hold child widgets.
	 *
	 * Base class for all widgets that can hold child widgets.
	 */
	
	class Container : public Widget
	{
		friend class MsgRouter;
	
		friend class Hook;
		friend class FlexHook;
		friend class ModalHook;
	
		friend class Menu;
		friend class Menubar;
		friend class Combobox;
	
		friend class RootPanel;
		friend class FlexPanel;
		friend class PopupLayer;
		friend class ModalLayer;
		friend class WgTablePanel;
		friend class ScrollPanel;
		friend class StackPanel;
		friend class VectorPanel;
		friend class ListPanel;
		friend class Capsule;
		friend class PackList;
	
		public:
	
			virtual bool			isInstanceOf( const char * pClassName ) const;
			virtual const char *	className( void ) const;
			static const char		CLASSNAME[];
			static Container_p	cast( const Object_p& pObject );
	
	
			bool					isContainer() const;
	
			inline Widget_p		firstWidget() const { return Widget_p(_firstWidget()); }
			inline Widget_p		lastWidget() const { return Widget_p(_lastWidget()); }
	
			inline Hook_p		firstHook() const { return _firstHook(); }
			inline Hook_p		lastHook() const { return _lastHook(); }
	
			virtual Widget_p		findWidget( const Coord& ofs, SearchMode mode ) { return Widget_p(_findWidget(ofs,mode)); }
	
	
			virtual bool			removeWidget( const Widget_p& pWidget ) = 0;
			virtual bool			clear() = 0;
				
		protected:
			Container();
			virtual ~Container() {};
			
			virtual bool			_isPanel() const;
	
			virtual Hook*			_firstHook() const = 0;
			virtual Hook*			_lastHook() const = 0;
	
			Widget *				_firstWidget() const;
			Widget *				_lastWidget() const;
	
	
			virtual Widget * 		_findWidget( const Coord& ofs, SearchMode mode );
			virtual void			_onStateChanged( State oldState );
	
			virtual void			_renderPatches( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, Patches * _pPatches );
			virtual Hook*			_firstHookWithGeo( Rect& geo ) const = 0;
			virtual Hook*			_nextHookWithGeo( Rect& geo, Hook * pHook ) const = 0;
	
			virtual Hook*			_lastHookWithGeo( Rect& geo ) const = 0;
			virtual Hook*			_prevHookWithGeo( Rect& geo, Hook * pHook ) const = 0;
	
			bool 					_focusRequested( Hook * pBranch, Widget * pWidgetRequesting );		// Needed until Panel inherits from Widget
			bool 					_focusReleased( Hook * pBranch, Widget * pWidgetReleasing );		// Needed until Panel inherits from Widget
	
			virtual ModalLayer *	_getModalLayer() const;
			virtual PopupLayer*	_getPopupLayer() const;
	
			virtual void	_onMaskPatches( Patches& patches, const Rect& geo, const Rect& clip, BlendMode blendMode );
			virtual void	_onCollectPatches( Patches& container, const Rect& geo, const Rect& clip );
			virtual void	_onCloneContent( const Container * _pOrg );
	
			bool			m_bSiblingsOverlap;	// Set if children (might be) overlapping each other (special considerations to be taken during rendering).
	
	};
	

} // namespace wg
#endif //WG_CONTAINER_DOT_H
