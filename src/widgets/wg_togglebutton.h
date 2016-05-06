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

#ifndef	WG_TOGGLEBUTTON_DOT_H
#define	WG_TOGGLEBUTTON_DOT_H

#ifndef	WG_WIDGET_DOT_H
#	include <wg_widget.h>
#endif

#ifndef WG_MODTEXT_DOT_H
#	include <wg_modtext.h>
#endif

#ifndef WG_ICON_DOT_H
#	include <wg_icon.h>
#endif

namespace wg 
{
	
	class	Surface;
	
	class ToggleButton;
	typedef	StrongPtr<ToggleButton,Widget_p>		ToggleButton_p;
	typedef	WeakPtr<ToggleButton,Widget_wp>	ToggleButton_wp;
	
	class ToggleGroup;
	typedef	StrongPtr<ToggleGroup,Object_p>		ToggleGroup_p;
	typedef	WeakPtr<ToggleGroup,Object_wp>	ToggleGroup_wp;
	
	
	/**
	 * @brief Combined ToggleButton, Checkbox and RadioButton widget.
	 *
	 * ToggleButton is a button widget that toggles its selected-state each time it is pressed.
	 * Press once to select, press again to deselect. ToggleButton is also used for Checkboxes
	 * and RadioButtons since these are technically just ToggleButtons with different skinning
	 * and possibly (depending on exact behavior desired) a different ClickArea.
	 *
	 * The ToggleButton is built up by three different visual elements: The button itself,
	 * the label and the icon. A typical StateButton will have the button itself skinned and
	 * the label set to some suitable text while a CheckBox and RadioButton will leave the button
	 * unskinned and use the icon and label for its visual representation.
	 *
	 **/
	
	class	ToggleButton : public Widget, protected IconHolder, protected TextHolder
	{
	friend class ToggleGroup;
	public:
		static ToggleButton_p	create() { return ToggleButton_p(new ToggleButton()); }
	
		bool						isInstanceOf( const char * pClassName ) const;
		const char *				className( void ) const;
		static const char			CLASSNAME[];
		static ToggleButton_p	cast( const Object_p& pObject );
	
		enum ClickArea
		{
			DEFAULT,		///< Full geometry of icon (no alpha test) + text + area between + alpha test on background.
			ALPHA,			///< Alpha test on background and icon.
			GEO,			///< Full geometry of Widget is clickable.
			ICON,			///< Only the icon (alpha test) is clickable.
			TEXT			///< Only the text (no alpha test) is clickable.
		};
	
	
		//____ Interfaces() ____________________________________
	
		ModText		label;
		Icon			icon;
	
	
		//____ Methods() _______________________________________
	
	 	void				setClickArea( ClickArea clickArea );
		inline ClickArea	getClickArea() const;
	
		inline bool			isSelected();
		virtual bool		setSelected( bool bSelected );
	
		void				setFlipOnRelease( bool bFlipOnRelease );
		inline bool			flipOnRelease();
	
		inline ToggleGroup_p toggleGroup() const;
		Size				preferredSize() const;
		inline bool			isAutoEllipsisDefault() const;
	
	
	protected:
		ToggleButton();
		virtual ~ToggleButton();
		virtual Widget* _newOfMyType() const { return new ToggleButton(); };
	
		void	_cloneContent( const Widget * _pOrg );
		void	_render( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, const Rect& _clip );
		void	_receive( const Msg_p& pMsg );
		void	_refresh();
		void	_setSize( const Size& size );
		bool	_alphaTest( const Coord& ofs );
		void	_setState( State state );
		void	_setSkin( const Skin_p& pSkin );
		
		Object * 		_object() { return this; };
		void			_onFieldDirty( Field * pField );
		void			_onFieldDirty( Field * pField, const Rect& rect );
		void 			_onFieldResize( Field * pField );
	
		void	_setToggleGroup( ToggleGroup * pGroup );
		ToggleGroup * _toggleGroup() const { return m_pToggleGroup.rawPtr(); }
	
	private:
	
		void	_refreshTextArea();
		bool	_markTestTextArea( int _x, int _y );
		
		bool			m_bPressed;						// Set when mouse is pressed and over.
		bool			m_bReturnPressed;
		bool			m_bFlipOnRelease;				// Set if we want to flip StateButton on press (default), not click.
	
		TextField		m_label;
		IconField		m_icon;
		ToggleGroup_p	m_pToggleGroup;
	
		ClickArea		m_clickArea;
	};
	
	
	inline ToggleButton::ClickArea ToggleButton::getClickArea() const
	{
		return m_clickArea;
	}
	
	inline bool ToggleButton::isSelected()
	{
		return m_state.isSelected();
	};
	
	inline bool ToggleButton::flipOnRelease()
	{
		return m_bFlipOnRelease;
	}
	
	inline ToggleGroup_p ToggleButton::toggleGroup() const
	{
		return m_pToggleGroup;
	}
	
	inline bool ToggleButton::isAutoEllipsisDefault() const
	{
		return false;
	};
	
	
	

} // namespace wg
#endif //	WG_TOGGLEBUTTON_DOT_H
