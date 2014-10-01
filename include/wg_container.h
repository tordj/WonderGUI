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
typedef	WgStrongPtr<WgContainer,WgWidgetPtr>			WgContainerPtr;
typedef	WgWeakPtr<WgContainer,WgWidgetWeakPtr>		WgContainerWeakPtr;

/**
 * @brief Base class for all widgets that can take child widgets.
 */

class WgContainer : public WgWidget
{
	friend class WgEventHandler;

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

	friend class WgRadioButton;

	public:

		virtual bool			IsInstanceOf( const char * pClassName ) const;
		virtual const char *	ClassName( void ) const;
		static const char		CLASSNAME[];
		static WgContainerPtr	Cast( const WgObjectPtr& pObject );


		bool					IsContainer() const;

		inline WgWidgetPtr		FirstWidget() const { return WgWidgetPtr(_firstWidget()); }
		inline WgWidgetPtr		LastWidget() const { return WgWidgetPtr(_lastWidget()); }

		inline WgHookPtr		FirstHook() const { return _firstHook(); }
		inline WgHookPtr		LastHook() const { return _lastHook(); }

		virtual WgWidgetPtr		FindWidget( const WgCoord& ofs, WgSearchMode mode ) { return WgWidgetPtr(_findWidget(ofs,mode)); }


		virtual bool			RemoveWidget( const WgWidgetPtr& pWidget ) = 0;
		virtual bool			Clear() = 0;
			
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
		virtual void	_onEvent( const WgEventPtr& pEvent, WgEventHandler * pHandler );

		bool			m_bSiblingsOverlap;	// Set if children (might be) overlapping each other (special considerations to be taken during rendering).
		WgWidget *		m_pHoveredChild;	// Careful not to use the pointer! Its widget might be destroyed.

};

#endif //WG_CONTAINER_DOT_H
