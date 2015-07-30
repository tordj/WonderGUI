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

class WgPatches;
class WgModalLayer;
class WgPopupLayer;

class WgContainer;
typedef	WgStrongPtr<WgContainer,WgWidget_p>			WgContainer_p;
typedef	WgWeakPtr<WgContainer,WgWidget_wp>		WgContainer_wp;

/**
 * @brief Base class for all widgets that can hold child widgets.
 *
 * Base class for all widgets that can hold child widgets.
 */

class WgContainer : public WgWidget
{
	friend class WgMsgRouter;

	friend class WgHook;
	friend class WgFlexHook;
	friend class WgModalHook;

	friend class WgMenu;
	friend class WgMenubar;
	friend class WgCombobox;

	friend class WgRootPanel;
	friend class WgFlexPanel;
	friend class WgPopupLayer;
	friend class WgModalLayer;
	friend class WgTablePanel;
	friend class WgScrollPanel;
	friend class WgStackPanel;
	friend class WgVectorPanel;
	friend class WgListPanel;
	friend class WgCapsule;
	friend class WgPackList;

	public:

		virtual bool			isInstanceOf( const char * pClassName ) const;
		virtual const char *	className( void ) const;
		static const char		CLASSNAME[];
		static WgContainer_p	cast( const WgObject_p& pObject );


		bool					IsContainer() const;

		inline WgWidget_p		firstWidget() const { return WgWidget_p(_firstWidget()); }
		inline WgWidget_p		lastWidget() const { return WgWidget_p(_lastWidget()); }

		inline WgHook_p		firstHook() const { return _firstHook(); }
		inline WgHook_p		lastHook() const { return _lastHook(); }

		virtual WgWidget_p		findWidget( const WgCoord& ofs, WgSearchMode mode ) { return WgWidget_p(_findWidget(ofs,mode)); }


		virtual bool			removeWidget( const WgWidget_p& pWidget ) = 0;
		virtual bool			clear() = 0;
			
	protected:
		WgContainer();
		virtual ~WgContainer() {};
		
		virtual bool			_isPanel() const;

		virtual WgHook*			_firstHook() const = 0;
		virtual WgHook*			_lastHook() const = 0;

		WgWidget *				_firstWidget() const;
		WgWidget *				_lastWidget() const;


		virtual WgWidget * 		_findWidget( const WgCoord& ofs, WgSearchMode mode );
		virtual void			_onStateChanged( WgState oldState );

		virtual void			_renderPatches( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, WgPatches * _pPatches );
		virtual WgHook*			_firstHookWithGeo( WgRect& geo ) const = 0;
		virtual WgHook*			_nextHookWithGeo( WgRect& geo, WgHook * pHook ) const = 0;

		virtual WgHook*			_lastHookWithGeo( WgRect& geo ) const = 0;
		virtual WgHook*			_prevHookWithGeo( WgRect& geo, WgHook * pHook ) const = 0;

		bool 					_focusRequested( WgHook * pBranch, WgWidget * pWidgetRequesting );		// Needed until WgPanel inherits from WgWidget
		bool 					_focusReleased( WgHook * pBranch, WgWidget * pWidgetReleasing );		// Needed until WgPanel inherits from WgWidget

		virtual WgModalLayer *	_getModalLayer() const;
		virtual WgPopupLayer*	_getPopupLayer() const;

		virtual void	_onMaskPatches( WgPatches& patches, const WgRect& geo, const WgRect& clip, WgBlendMode blendMode );
		virtual void	_onCollectPatches( WgPatches& container, const WgRect& geo, const WgRect& clip );
		virtual void	_onCloneContent( const WgContainer * _pOrg );

		bool			m_bSiblingsOverlap;	// Set if children (might be) overlapping each other (special considerations to be taken during rendering).

};

#endif //WG_CONTAINER_DOT_H
