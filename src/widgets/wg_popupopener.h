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
#ifndef WG_POPUPOPENER_DOT_H
#define WG_POPUPOPENER_DOT_H
#pragma once

#include <wg_widget.h>
#include <wg_child.h>
#include <wg_textitem.h>
#include <wg_modtext.h>
#include <wg_icon.h>

namespace wg
{
	class PopupOpener;
	typedef	StrongPtr<PopupOpener>	PopupOpener_p;
	typedef	WeakPtr<PopupOpener>	PopupOpener_wp;


	class PopupSlot	/** @private */			// Note, does not inherit from Slot
	{
	public:
		Widget_p	pWidget;
	};


	class PopupOpener : public Widget, protected ChildHolder
	{
	public:

		//.____ Creation __________________________________________

		static PopupOpener_p	create() { return PopupOpener_p(new PopupOpener()); }

		//.____ Interfaces _______________________________________

		Child<PopupSlot, ChildHolder>	popup;
		ModText			label;
		Icon			icon;

		//.____ Identification __________________________________________

		bool					isInstanceOf(const char * pClassName) const;
		const char *			className(void) const;
		static const char		CLASSNAME[];
		static PopupOpener_p	cast(Object * pObject);

		//.____ Behavior ____________________________________________

		void		setOpenOnHover(bool bOpen);
		bool		openOnHover() const { return m_bOpenOnHover;  }


	protected:
		PopupOpener();
		virtual ~PopupOpener();
		virtual Widget* _newOfMyType() const { return new PopupOpener(); };

		void		_setWidget(Slot * pSlot, Widget * pNewWidget);
		Object *	_object();



		Widget *	m_pPopup;
		Origo		m_attachPoint;
		bool		m_bOpenOnHover;

	};


}


#endif //WG_POPUPOPENER_DOT_H