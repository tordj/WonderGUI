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

		CTextDisplay		label;
		CIconDisplay		icon;

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Geometry ______________________________________________________

		virtual MU		matchingHeight(MU width) const override;

		Size			preferredSize() const override;


	protected:
		Button();
		virtual ~Button();
		virtual Widget* _newOfMyType() const override { return new Button(); };

		//	virtual int		_matchingWidth( int height ) const override;



		virtual void	_receive( Msg * pMsg ) override;
		virtual void	_render( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window) override;
		void			_refresh() override;
		virtual void	_cloneContent( const Widget * _pOrg ) override;
		bool			_alphaTest( const Coord& ofs ) override;
		virtual void	_resize( const Size& size ) override;
		void			_setState( State state ) override;


		Coord			_componentPos( const GeoComponent * pComponent ) const override;
		Size			_componentSize( const GeoComponent * pComponent ) const override;
		Rect			_componentGeo( const GeoComponent * pComponent ) const override;

		class TextAccess : public CTextDisplay { friend class Button; };
		const TextAccess& _text() const { return static_cast<const TextAccess&>(label); }
		TextAccess& _text() { return static_cast<TextAccess&>(label); }

		class IconAccess : public CIconDisplay { friend class Button; };
		const IconAccess& _icon() const { return static_cast<const IconAccess&>(icon); }
		IconAccess& _icon() { return static_cast<IconAccess&>(icon); }


		bool			m_bPressed;				// Set when left mousebutton was pressed inside.
		bool			m_bReturnPressed;
	};


} // namespace wg
#endif //WG_BUTTON_DOT_H
