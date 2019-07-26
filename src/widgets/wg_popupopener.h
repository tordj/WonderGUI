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
#include <wg_ichild.h>
#include <wg_ctextdisplay.h>
#include <wg_itextdisplay.h>
#include <wg_iicondisplay.h>

namespace wg
{
	class PopupOpener;
	typedef	StrongPtr<PopupOpener>	PopupOpener_p;
	typedef	WeakPtr<PopupOpener>	PopupOpener_wp;



	class PopupOpener : public Widget
	{
	public:

		//.____ Creation __________________________________________

		static PopupOpener_p	create() { return PopupOpener_p(new PopupOpener()); }

		//.____ Interfaces _______________________________________

		ITextDisplay			label;
		IIconDisplay			icon;

		//.____ Identification __________________________________________

		bool					isInstanceOf(const char * pClassName) const override;
		const char *			className(void) const override;
		static const char		CLASSNAME[];
		static PopupOpener_p	cast(Object * pObject);

		//.____ Behavior ____________________________________________

		void		setPopup(Widget * pPopup);
		Widget_p	popup() const { return m_pPopup;  }

		void		setOpenOnHover(bool bOpen);
		bool		openOnHover() const { return m_bOpenOnHover;  }

		void		setAttachPoint(Origo attachPoint);
		Origo		attachPoint() const { return m_attachPoint;  }

	protected:
		PopupOpener();
		virtual ~PopupOpener();
		virtual Widget* _newOfMyType() const override { return new PopupOpener(); };

		virtual int		_matchingHeight(int width) const override;
		//	virtual int		_matchingWidth( int height ) const override;

		SizeI			_preferredSize() const override;

		void			_cloneContent(const Widget * _pOrg) override;
		void			_render(GfxDevice * pDevice, const RectI& _canvas, const RectI& _window) override;
		void			_setSize(const SizeI& size) override;
		void			_refresh() override;
		void			_receive(Msg * pMsg) override;
		void			_setState(State state) override;
		void			_setSkin(Skin * pSkin) override;


		void			_open();
		void			_close();

		CoordI			_componentPos(const Component * pComponent) const override;
		SizeI			_componentSize(const Component * pComponent) const override;
		RectI			_componentGeo(const Component * pComponent) const override;



		Widget_p		m_pPopup;
		CTextDisplay		m_text;
		CIconDisplay		m_icon;

		Origo			m_attachPoint;
		bool			m_bOpenOnHover;
		bool			m_bOpen;
		State			m_closeState;
	};


}


#endif //WG_POPUPOPENER_DOT_H
