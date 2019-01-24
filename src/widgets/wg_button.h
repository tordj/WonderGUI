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
#include <wg_textitem.h>
#include <wg_modtext.h>
#include <wg_icon.h>

namespace wg 
{
	
	class Button;
	typedef	StrongPtr<Button>		Button_p;
	typedef	WeakPtr<Button>	Button_wp;
	
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

		ModText			label;
		Icon			icon;

		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static Button_p	cast( Object * pObject );
		
		//.____ Behavior ____________________________________________
	
		void			setDownWhenMouseOutside( bool bDown );					///< @brief Set if button should stay in pressed state even if mouse goes outside.
		bool			downWhenMouseOutside() const { return m_bDownOutside; }	///< @brief Check if button will stay in pressed state even if mouse goes outside.

		//.____ Geometry ____________________________________________
		
		virtual int		matchingHeight( int width ) const;
	//	virtual int		matchingWidth( int height ) const;
	
		Size			preferredSize() const;	
	
	protected:
		Button();
		virtual ~Button();
		virtual Widget* _newOfMyType() const { return new Button(); };
	
		virtual void	_receive( Msg * pMsg );
		virtual void	_render( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window);
		void			_refresh();
		virtual void	_cloneContent( const Widget * _pOrg );
		bool			_alphaTest( const Coord& ofs );
		virtual void	_setSize( const Size& size );
		void			_setState( State state );
	
	
		void			_renderRequested( Item * pItem );
		void			_renderRequested( Item * pItem, const Rect& rect );
		void 			_resizeRequested( Item * pItem );
	
		Coord			_itemPos( const Item * pItem ) const;
		Size			_itemSize( const Item * pItem ) const;
		Rect			_itemGeo( const Item * pItem ) const;
	
	
	
		TextItem		m_text;
		IconItem		m_icon;
	
		bool			m_bDownOutside;			// Button remains down when pressed and mouse gets outside?
	
		bool			m_bPressed;				// Set when left mousebutton was pressed inside.
		bool			m_bReturnPressed;
	};
	

} // namespace wg
#endif //WG_BUTTON_DOT_H
