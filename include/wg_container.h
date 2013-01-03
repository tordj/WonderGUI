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

#ifndef WG_WIDGETHOLDER_DOT_H
#	include <wg_widgetholder.h>
#endif

class WgPatches;

class WgContainer : public WgWidgetHolder, public WgWidget
{
	friend class WgEventHandler;

	friend class WgHook;
	friend class WgFlexHook;
	friend class WgModalHook;

	friend class WgRootPanel;
	friend class WgFlexPanel;
	friend class WgModalLayer;
	friend class WgTablePanel;
	friend class WgScrollPanel;
	friend class WgStackPanel;
	friend class WgSortablePanel;
	friend class WgVPanel;
	friend class WgCapsule;

	public:


		virtual WgWidget * 	FindWidget( const WgCoord& ofs, WgSearchMode mode );

		bool		IsWidget() const;
		bool		IsContainer() const;

		WgWidget *				CastToWidget();
		const WgWidget *		CastToWidget() const;
		WgContainer *			CastToContainer();
		const WgContainer *		CastToContainer() const;

		// The following need to be here to tackle ambigious access to identical methods in WgWidget and WgWidgetHolder.

		virtual bool			IsPanel() const;
		virtual WgPanel *		CastToPanel();
		virtual const WgPanel *	CastToPanel() const;

		virtual bool			IsCapsule() const;
		virtual WgCapsule *		CastToCapsule();
		virtual const WgCapsule *	CastToCapsule() const;

		virtual bool			IsLayer() const;
		virtual WgLayer *		CastToLayer();
		virtual const WgLayer *	CastToLayer() const;


	protected:
		WgContainer();
		virtual ~WgContainer() {};

		virtual void	_onEnable();
		virtual void	_onDisable();

		virtual void	_renderPatches( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, WgPatches * _pPatches, Uint8 _layer );
		virtual WgHook* _firstHookWithGeo( WgRect& geo ) const = 0;
		virtual WgHook* _nextHookWithGeo( WgRect& geo, WgHook * pHook ) const = 0;

		virtual WgHook* _lastHookWithGeo( WgRect& geo ) const = 0;
		virtual WgHook* _prevHookWithGeo( WgRect& geo, WgHook * pHook ) const = 0;

		bool 			_focusRequested( WgHook * pBranch, WgWidget * pWidgetRequesting );	// Needed until WgPanel inherits from WgWidget
		bool 			_focusReleased( WgHook * pBranch, WgWidget * pWidgetReleasing );		// Needed until WgPanel inherits from WgWidget

		WgModalLayer *	_getModalLayer() const;
		WgMenuLayer*	_getMenuLayer() const;

		virtual void	_onMaskPatches( WgPatches& patches, const WgRect& geo, const WgRect& clip, WgBlendMode blendMode );
		virtual void	_onCollectPatches( WgPatches& container, const WgRect& geo, const WgRect& clip );
		virtual bool 	_onAlphaTest( const WgCoord& ofs );
		virtual void	_onCloneContent( const WgContainer * _pOrg );

		bool		m_bSiblingsOverlap;	// Set if children (might be) overlapping each other (special considerations to be taken during rendering).
};

#endif //WG_CONTAINER_DOT_H
