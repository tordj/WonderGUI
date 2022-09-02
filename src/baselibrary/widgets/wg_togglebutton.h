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
#pragma once

#include <wg_widget.h>
#include <wg_ctextdisplay.h>
#include <wg_cicondisplay.h>

namespace wg
{

	class	Surface;

	class ToggleButton;
	typedef	StrongPtr<ToggleButton>		ToggleButton_p;
	typedef	WeakPtr<ToggleButton>	ToggleButton_wp;

	class ToggleGroup;
	typedef	StrongPtr<ToggleGroup>		ToggleGroup_p;
	typedef	WeakPtr<ToggleGroup>	ToggleGroup_wp;


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

	class	ToggleButton : public Widget
	{
	friend class ToggleGroup;
	public:
		enum class ClickArea
		{
			Default,		///< Full geometry of icon (no alpha test) + text + area between + alpha test on background.
			Alpha,			///< Alpha test on background and icon.
			Geo,			///< Full geometry of Widget is clickable.
			Icon,			///< Only the icon (alpha test) is clickable.
			Text			///< Only the text (no alpha test) is clickable.
		};

		//____ Blueprint ______________________________________________________

		struct Blueprint
		{
			Object_p		baggage;
			ClickArea		clickArea = ClickArea::Default;
			bool			dropTarget = false;
			bool			enabled = true;
			Finalizer_p		finalizer;
			bool			flipOnRelease = false;
			CIconDisplay::Blueprint	icon;
			int				id = 0;
			CTextDisplay::Blueprint	label;
			MarkPolicy		markPolicy = MarkPolicy::AlphaTest;
			bool			pickable = false;
			int				pickCategory = 0;
			PointerStyle	pointer = PointerStyle::Default;
			int				scale = -1;
			bool			selected = false;
			bool			selectable = true;
			Skin_p			skin;
			bool			tabLock = false;
			String			tooltip;
		};

		//.____ Creation __________________________________________

		static ToggleButton_p	create() { return ToggleButton_p(new ToggleButton()); }
		static ToggleButton_p	create(const Blueprint& blueprint) { return ToggleButton_p(new ToggleButton(blueprint)); }

		//.____ Components ____________________________________

		CTextDisplay		label;
		CIconDisplay		icon;

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ State _________________________________________________

		inline bool			isSelected();
		virtual bool		setSelected( bool bSelected );

		//.____ Behavior _______________________________________________________

	 	void				setClickArea( ClickArea clickArea );
		inline ClickArea	clickArea() const;

		void				setFlipOnRelease( bool bFlipOnRelease );
		inline bool			flipOnRelease();

		//.____ Misc _________________________________________________________________

		inline ToggleGroup_p toggleGroup() const;

		//.____ Geometry _________________________________________________

		SizeSPX			_defaultSize(int scale = -1) const override;


	protected:
		ToggleButton();
		ToggleButton(const Blueprint& bp);
		virtual ~ToggleButton();

		void		_render( GfxDevice * pDevice, const RectSPX& _canvas, const RectSPX& _window ) override;
		void		_receive( Msg * pMsg ) override;
		void		_resize( const SizeSPX& size, int scale = -1 ) override;
		bool		_alphaTest( const CoordSPX& ofs ) override;
		void		_setState(State state) override { _setState(state, false); }
		void		_setState( State state, bool bPostMsg );

		void		_setToggleGroup( ToggleGroup * pGroup );
		ToggleGroup * _toggleGroup() const { return m_pToggleGroup.rawPtr(); }

		CoordSPX	_componentPos( const WidgetComponent * pComponent ) const override;
		SizeSPX		_componentSize( const WidgetComponent * pComponent ) const override;
		RectSPX		_componentGeo( const WidgetComponent * pComponent ) const override;



	private:
		class IconAccess : public CIconDisplay { friend class ToggleButton; };
		const IconAccess& _icon() const { return static_cast<const IconAccess&>(icon); }
		IconAccess& _icon() { return static_cast<IconAccess&>(icon); }


//		void	_refreshTextArea();
		bool	_markTestTextArea( const CoordSPX& pos );

		bool			m_bPressed;						// Set when mouse is pressed and over.
		bool			m_bReturnPressed;
		bool			m_bFlipOnRelease;				// Set if we want to flip StateButton on press (default), not click.

		ToggleGroup_p	m_pToggleGroup;

		ClickArea		m_clickArea;
	};


	inline ToggleButton::ClickArea ToggleButton::clickArea() const
	{
		return m_clickArea;
	}

	inline void ToggleButton::setClickArea( ClickArea clickArea )
	{
		m_clickArea = clickArea;
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




} // namespace wg
#endif //	WG_TOGGLEBUTTON_DOT_H
