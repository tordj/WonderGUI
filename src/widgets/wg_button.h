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
#include <wg_itextdisplay.h>
#include <wg_iicondisplay.h>

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

		//.____ Interfaces _______________________________________

		ITextDisplay		label;
		IIconDisplay			icon;

		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const override;
		const char *		className( void ) const override;
		static const char	CLASSNAME[];
		static Button_p	cast( Object * pObject );

		//.____ Behavior ____________________________________________

		void			setDownWhenMouseOutside( bool bDown );					///< @brief Set if button should stay in pressed state even if mouse goes outside.
		bool			downWhenMouseOutside() const { return m_bDownOutside; }	///< @brief Check if button will stay in pressed state even if mouse goes outside.

	protected:
		Button();
		virtual ~Button();
		virtual Widget* _newOfMyType() const override { return new Button(); };

		virtual int		_matchingHeight(int width) const override;
		//	virtual int		_matchingWidth( int height ) const override;

		SizeI			_preferredSize() const override;


		virtual void	_receive( Msg * pMsg ) override;
		virtual void	_render( GfxDevice * pDevice, const RectI& _canvas, const RectI& _window) override;
		void			_refresh() override;
		virtual void	_cloneContent( const Widget * _pOrg ) override;
		bool			_alphaTest( const CoordI& ofs ) override;
		virtual void	_setSize( const SizeI& size ) override;
		void			_setState( State state ) override;


		CoordI			_componentPos( const Component * pComponent ) const override;
		SizeI			_componentSize( const Component * pComponent ) const override;
		RectI			_componentGeo( const Component * pComponent ) const override;



		CTextDisplay		m_text;
		CIconDisplay		m_icon;

		bool			m_bDownOutside;			// Button remains down when pressed and mouse gets outside?

		bool			m_bPressed;				// Set when left mousebutton was pressed inside.
		bool			m_bReturnPressed;
	};


} // namespace wg
#endif //WG_BUTTON_DOT_H
