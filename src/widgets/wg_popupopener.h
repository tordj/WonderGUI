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

		bool					isInstanceOf(const char * pClassName) const;
		const char *			className(void) const;
		static const char		CLASSNAME[];
		static PopupOpener_p	cast(Object * pObject);

		//.____ Behavior ____________________________________________

		void		setPopup(Widget * pPopup);
		Widget_p	popup() const { return m_pPopup;  }

		void		setOpenOnHover(bool bOpen);
		bool		openOnHover() const { return m_bOpenOnHover;  }

		void		setAttachPoint(Origo attachPoint);
		Origo		attachPoint() const { return m_attachPoint;  }

		//.____ Geometry ____________________________________________

		virtual int		matchingHeight(int width) const;
		//	virtual int		matchingWidth( int height ) const;

		Size			preferredSize() const;


	protected:
		PopupOpener();
		virtual ~PopupOpener();
		virtual Widget* _newOfMyType() const { return new PopupOpener(); };

		void			_cloneContent(const Widget * _pOrg);
		void			_render(GfxDevice * pDevice, const Rect& _canvas, const Rect& _window);
		void			_setSize(const Size& size);
		void			_refresh();
		void			_receive(Msg * pMsg);
		void			_setState(State state);
		void			_setSkin(Skin * pSkin);


		void			_open();
		void			_close();

		void			_renderRequested(Component * pComponent);
		void			_renderRequested(Component * pComponent, const Rect& rect);
		void 			_resizeRequested(Component * pComponent);

		Coord			_componentPos(const Component * pComponent) const override;
		Size			_componentSize(const Component * pComponent) const override;
		Rect			_componentGeo(const Component * pComponent) const override;



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
