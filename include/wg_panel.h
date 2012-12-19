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

#ifndef	WG_PANEL_DOT_H
#define	WG_PANEL_DOT_H

#ifndef WG_WIDGET_DOT_H
#	include <wg_widget.h>
#endif

#ifndef WG_WIDGETCONTAINER_DOT_H
#	include <wg_widgetcontainer.h>
#endif

class WgPatches;

class WgPanel : public WgWidgetContainer, public WgWidget
{
	friend class WgEventHandler;

	friend class WgHook;
	friend class WgFlexHook;
	friend class WgModalHook;

	friend class WgRootPanel;
	friend class WgFlexPanel;
	friend class WgModalPanel;
	friend class WgTablePanel;
	friend class WgScrollPanel;
	friend class WgStackPanel;
	friend class WgSortablePanel;
	friend class WgVPanel;
	friend class WgMonoPanel;

	public:

		void		SetFocusGroup( bool bFocusGroup ) { m_bFocusGroup = bFocusGroup; }
		bool		IsFocusGroup() const { return m_bFocusGroup; }

		void		SetRadioGroup( bool bRadioGroup ) { m_bRadioGroup = bRadioGroup; }
		bool		IsRadioGroup() const { return m_bRadioGroup; }

		void		SetTooltipGroup( bool bTooltipGroup ) { m_bTooltipGroup = bTooltipGroup; }
		bool		IsTooltipGroup() const { return m_bTooltipGroup; }

		void		SetMaskOp( WgMaskOp operation );
		WgMaskOp	MaskOp() const { return m_maskOp; }

		virtual WgWidget * 	FindWidget( const WgCoord& ofs, WgSearchMode mode );

		bool		IsWidget() const;
		bool		IsPanel() const;
		bool		IsRoot() const;

		WgWidget *			CastToWidget();
		const WgWidget *		CastToWidget() const;
		WgPanel *	CastToPanel();
		const WgPanel * CastToPanel() const;
		WgRootPanel *			CastToRoot();
		const WgRootPanel *		CastToRoot() const;

	protected:
		WgPanel();
		virtual ~WgPanel() {};

		virtual void	_onEnable();
		virtual void	_onDisable();

		virtual void	_renderPatches( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, WgPatches * _pPatches, Uint8 _layer );
		virtual WgHook* _firstHookWithGeo( WgRect& geo ) const = 0;
		virtual WgHook* _nextHookWithGeo( WgRect& geo, WgHook * pHook ) const = 0;

		virtual WgHook* _lastHookWithGeo( WgRect& geo ) const = 0;
		virtual WgHook* _prevHookWithGeo( WgRect& geo, WgHook * pHook ) const = 0;

		bool 			_focusRequested( WgHook * pBranch, WgWidget * pWidgetRequesting );	// Needed until WgPanel inherits from WgWidget
		bool 			_focusReleased( WgHook * pBranch, WgWidget * pWidgetReleasing );		// Needed until WgPanel inherits from WgWidget

		WgModalPanel *	_getModalPanel() const;
		WgMenuPanel*	_getMenuPanel() const;

		virtual void	_onMaskPatches( WgPatches& patches, const WgRect& geo, const WgRect& clip, WgBlendMode blendMode );
		virtual void	_onCollectPatches( WgPatches& container, const WgRect& geo, const WgRect& clip );
		virtual bool 	_onAlphaTest( const WgCoord& ofs );
		virtual void	_onCloneContent( const WgPanel * _pOrg );

		bool		m_bFocusGroup;
		bool		m_bRadioGroup;
		bool		m_bTooltipGroup;	// All descendants belongs to the same tooltip group.
		bool		m_bSiblingsOverlap;	// Set if children (might be) overlapping each other (special considerations to be taken during rendering).
		WgMaskOp	m_maskOp;			// Specifies how container masks background.
};



#endif //WG_PANEL_DOT_H
