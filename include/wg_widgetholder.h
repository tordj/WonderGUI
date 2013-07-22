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

#ifndef	WG_WIDGETHOLDER_DOT_H
#define	WG_WIDGETHOLDER_DOT_H

#ifndef WG_TYPES_DOT_H
#	include <wg_types.h>
#endif

#ifndef WG_WIDGET_DOT_H
#	include <wg_widget.h>
#endif


class WgRootPanel;
class WgHook;
class WgContainer;
class WgPanel;
class WgList;
class WgCapsule;
class WgLayer;
class WgModalLayer;
class WgMenuLayer;
class WgCoord;

class WgWidgetHolder
{
	friend class WgPanel;
	friend class WgHook;
	friend class WgContainer;
	friend class WgRadioButton;
	public:
		WgWidgetPtr			FirstWidget() const { return WgWidgetPtr(_firstWidget()); }
		WgWidgetPtr			LastWidget() const { return WgWidgetPtr(_lastWidget()); }

		inline WgHook *		FirstHook() const { return _firstHook(); }
		inline WgHook *		LastHook() const { return _lastHook(); }

		virtual WgWidgetPtr FindWidget( const WgCoord& ofs, WgSearchMode mode ) { return WgWidgetPtr(_findWidget(ofs,mode)); }


		virtual bool		RemoveChild( const WgWidgetPtr& pWidget ) = 0;
		virtual bool		Clear() = 0;

		virtual WgWidget *					CastToWidget();
		virtual const WgWidget *			CastToWidget() const;
		virtual WgContainer *				CastToContainer();
		virtual const WgContainer *			CastToContainer() const;
		virtual WgPanel *					CastToPanel();
		virtual const WgPanel *				CastToPanel() const;
		virtual WgList *					CastToList();
		virtual const WgList *				CastToList() const;
		virtual WgCapsule *					CastToCapsule();
		virtual const WgCapsule *			CastToCapsule() const;
		virtual WgLayer *					CastToLayer();
		virtual const WgLayer *				CastToLayer() const;
		virtual WgRootPanel *				CastToRoot();
		virtual const WgRootPanel *			CastToRoot() const;

		virtual bool		IsRoot() const;
		virtual bool		IsWidget() const;
		virtual bool		IsContainer() const;
		virtual bool		IsPanel() const;
		virtual bool		IsList() const;
		virtual bool		IsCapsule() const;
		virtual bool		IsLayer() const;

	protected:

		virtual bool 		_focusRequested( WgHook * pBranch, WgWidget * pWidgetRequesting ) = 0;
		virtual bool 		_focusReleased( WgHook * pBranch, WgWidget * pWidgetReleasing ) = 0;

		virtual WgHook*	_firstHook() const = 0;
		virtual WgHook*	_lastHook() const = 0;

		WgWidget *		_firstWidget() const;
		WgWidget *		_lastWidget() const;
		virtual WgWidget * _findWidget( const WgCoord& ofs, WgSearchMode mode ) = 0;

};


#endif //WG_WIDGETHOLDER_DOT_H
