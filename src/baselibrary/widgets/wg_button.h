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
#ifndef WG_BUTTON_DOT_H
#define WG_BUTTON_DOT_H
#pragma once

#include <wg_widget.h>
#include <wg_ctextdisplay.h>
#include <wg_cicondisplay.h>

namespace wg
{

	class Button;
	typedef	StrongPtr<Button>		Button_p;
	typedef	WeakPtr<Button>			Button_wp;

	//____ Button ____________________________________________________________
	/**
	 * @brief A normal push button widget.
	 *
	 * A simple push button widget with label and icon.
	 **/

	class Button : public Widget
	{
	public:

		//.____ Creation __________________________________________

		static Button_p	create() { return Button_p(new Button()); }

		//.____ Components _______________________________________

		CTextDisplay		text;
		CIconDisplay		icon;

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Internal ______________________________________________________

		spx				_matchingHeight(spx width, int scale = -1) const override;
		SizeSPX			_preferredSize(int scale = -1) const override;


	protected:
		Button();
		virtual ~Button();
		virtual Widget* _newOfMyType() const override { return new Button(); };

		//	virtual int		_matchingWidth( int height ) const override;



		void			_receive( Msg * pMsg ) override;
		void			_render( GfxDevice * pDevice, const RectSPX& _canvas, const RectSPX& _window) override;
		void			_refresh() override;
		void			_cloneContent( const Widget * _pOrg ) override;
		bool			_alphaTest( const CoordSPX& ofs ) override;
		void			_resize( const SizeSPX& size, int scale = -1 ) override;
		void			_setState( State state ) override;


		CoordSPX		_componentPos( const WidgetComponent * pComponent ) const override;
		SizeSPX			_componentSize( const WidgetComponent * pComponent ) const override;
		RectSPX			_componentGeo( const WidgetComponent * pComponent ) const override;

		class IconAccess : public CIconDisplay { friend class Button; };
		const IconAccess& _icon() const { return static_cast<const IconAccess&>(icon); }
		IconAccess& _icon() { return static_cast<IconAccess&>(icon); }


		bool			m_bPressed;				// Set when left mousebutton was pressed inside.
		bool			m_bReturnPressed;
	};


} // namespace wg
#endif //WG_BUTTON_DOT_H
