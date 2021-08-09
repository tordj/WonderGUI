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

#ifndef	WG_WIDGETCOMPONENT_DOT_H
#define	WG_WIDGETCOMPONENT_DOT_H
#pragma once

#include <wg_component.h>
#include <wg_widget.h>

namespace wg
{

	//____ WidgetComponent _________________________________________________________________

	class WidgetComponent : public Component		/** @private */
	{
	public:

		WidgetComponent( Widget * pWidget ) : m_pWidget(pWidget) {}

		//.____ Identification _________________________________________________

		const TypeInfo& typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

	protected:
		inline CoordSPX	_pos() const { return m_pWidget->_componentPos(this); }
		inline int		_scale() const { return m_pWidget->_scale(); }
		inline SizeSPX	_size() const { return m_pWidget->_componentSize(this); }
		inline RectSPX	_geo() const { return m_pWidget->_componentGeo(this); }
		inline CoordSPX	_globalPos() const { return m_pWidget->_globalComponentPos(this); }
		inline RectSPX	_globalGeo() const { return m_pWidget->_globalComponentGeo(this); }


		inline void		_requestRender() const { m_pWidget->_componentRequestRender(this); }
		inline void		_requestRender(const RectSPX& rect) const { m_pWidget->_componentRequestRender(this, rect); }
		inline void		_requestResize() const { m_pWidget->_componentRequestResize(this); }
		inline void		_requestVisibility() const { m_pWidget->_componentRequestInView(this); }
		inline void		_requestVisibility(const RectSPX& mustHave, const RectSPX& niceToHave) const { m_pWidget->_componentRequestInView(this, mustHave, niceToHave); }
		inline void		_requestFocus() const { m_pWidget->_componentRequestFocus(this); }

		inline void		_notify(ComponentNotif notification, int value, void * pData) { m_pWidget->_receiveComponentNotif(this, notification, value, pData); }

		Object *		_object() override;
		const Object *	_object() const override;

	protected:
		Widget *		m_pWidget;
	};


} // namespace wg
#endif //WG_WIDGETCOMPONENT_DOT_H
