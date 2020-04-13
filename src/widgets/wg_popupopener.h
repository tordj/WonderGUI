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
#include <wg_ctextdisplay.h>
#include <wg_cicondisplay.h>

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

		//.____ Components _______________________________________

		CTextDisplay			label;
		CIconDisplay			icon;

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Geometry ______________________________________________________

		virtual MU		matchingHeight(MU width) const override;

		Size			preferredSize() const override;


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

		void			_cloneContent(const Widget * _pOrg) override;
		void			_render(GfxDevice * pDevice, const Rect& _canvas, const Rect& _window) override;
		void			_resize(const Size& size) override;
		void			_refresh() override;
		void			_receive(Msg * pMsg) override;
		void			_setState(State state) override;
		void			_setSkin(Skin * pSkin) override;


		void			_open();
		void			_close();

		Coord			_componentPos(const GeoComponent * pComponent) const override;
		Size			_componentSize(const GeoComponent * pComponent) const override;
		Rect			_componentGeo(const GeoComponent * pComponent) const override;


		class TextAccess : public CTextDisplay { friend class PopupOpener; };
		const TextAccess& _label() const { return static_cast<const TextAccess&>(label); }
		TextAccess& _label() { return static_cast<TextAccess&>(label); }

		class IconAccess : public CIconDisplay { friend class PopupOpener; };
		const IconAccess& _icon() const { return static_cast<const IconAccess&>(icon); }
		IconAccess& _icon() { return static_cast<IconAccess&>(icon); }



		Widget_p		m_pPopup;

		Origo			m_attachPoint = Origo::SouthWest;
		bool			m_bOpenOnHover = false;
		bool			m_bOpen = false;
		State			m_closeState;
	};


}


#endif //WG_POPUPOPENER_DOT_H
