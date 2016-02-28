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

#ifndef WG_WIDGET_DOT_H
#	include <wg_widget.h>
#endif

#ifndef	WG_TEXTFIELD_DOT_H
#	include <wg_textfield.h>
#endif

#ifndef WG_MODTEXT_DOT_H
#	include <wg_modtext.h>
#endif

#ifndef	WG_ICON_DOT_H
#	include <wg_icon.h>
#endif

namespace wg 
{
	
	class Button;
	typedef	StrongPtr<Button,Widget_p>		Button_p;
	typedef	WeakPtr<Button,Widget_wp>	Button_wp;
	
	//____ Button ____________________________________________________________
	/**
	 * @breif A normal push button widget.
	 *
	 * A simple push button widget with label and icon.
	 **/
	
	class Button : public Widget, protected IconHolder, protected TextHolder
	{
	public:
		static Button_p	create() { return Button_p(new Button()); }
	
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static Button_p	cast( const Object_p& pObject );
	
		//____ Interfaces ______________________________________
	
		ModText			label;
		Icon			icon;
	
		//____ Methods __________________________________________
	
		void			setDownWhenMouseOutside( bool bDown );					///< @brief Set if button should stay in pressed state even if mouse goes outside.
		bool			downWhenMouseOutside() const { return m_bDownOutside; }	///< @brief Check if button will stay in pressed state even if mouse goes outside.
		
		virtual int		matchingHeight( int width ) const;
	//	virtual int		matchingWidth( int height ) const;
	
		Size			preferredSize() const;
	
		bool			isAutoEllipsisDefault() const { return false; };
	
	
	protected:
		Button();
		virtual ~Button();
		virtual Widget* _newOfMyType() const { return new Button(); };
	
		virtual void	_onMsg( const Msg_p& pMsg );
		virtual void	_onRender( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, const Rect& _clip );
		void			_onRefresh();
		virtual void	_onCloneContent( const Widget * _pOrg );
		bool			_onAlphaTest( const Coord& ofs, const Size& sz );
		virtual void	_onNewSize( const Size& size );
		void			_onStateChanged( State oldState );
		virtual void	_onSkinChanged( const Skin_p& pOldSkin, const Skin_p& pNewSkin );
	
	
		Object * 		_object() { return this; };
		void			_onFieldDirty( Field * pField );
		void			_onFieldDirty( Field * pField, const Rect& rect );
		void 			_onFieldResize( Field * pField );
	
		TextField		m_text;
		IconField		m_icon;
	
		bool			m_bDownOutside;			// Button remains down when pressed and mouse gets outside?
	
		bool			m_bPressed;				// Set when left mousebutton was pressed inside.
		bool			m_bReturnPressed;
	};
	

} // namespace wg
#endif //WG_BUTTON_DOT_H
