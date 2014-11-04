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

#ifndef	WG_SCROLLBAR_DOT_H
#define	WG_SCROLLBAR_DOT_H

#ifndef	WG_WIDGET_DOT_H
#	include <wg_widget.h>
#endif

#ifndef WG_SKIN_DOT_H
#	include <wg_skin.h>
#endif


class WgSurface;
class WgScrollbarTarget;
class WgMenu;


class WgScrollbar;
typedef	WgStrongPtr<WgScrollbar,WgWidgetPtr>		WgScrollbarPtr;
typedef	WgWeakPtr<WgScrollbar,WgWidgetWeakPtr>		WgScrollbarWeakPtr;

//____ Class: WgScrollbar _____________________________________________________
/**
 * @brief Scrollbar widget.
 *
 * WgScrollbar is a normal scrollbar widget with buttons for stepping forward/backward
 * and a resizable handle that can be dragged to move the view.
 *
 * There can be a total of four buttons and a handle (slider) in the scrollbar. The buttons
 * are optional and can be enabled individually. They consists of a forward and backward button on each side
 * of the slider.
 *
 * The same scrollbar class is used for vertical and horizontal scrollbars with the orientation
 * setting deciding if scrollbar is displayed vertically or horizontally.
 *
 * Skinning a scrollbar is a little more complex since it consists of five individual skins. The first skin
 * is the normal widget skin which encloses all components of the scrollbar. The two next skins are the skins
 * for the forward and backward buttons. The fourth skin is the "background skin" which fills the area between
 * the buttons in the header and the footer (the area the handle can move over). The fifth and final skin is
 * the skin for the dragbar handle.
 *
 **/

class	WgScrollbar : public WgWidget
{
	friend class WgScrollbarTarget;
	friend class WgMenu;

	public:
		static WgScrollbarPtr	Create() { return WgScrollbarPtr(new WgScrollbar()); }

		bool		IsInstanceOf( const char * pClassName ) const;
		const char *ClassName( void ) const;
		static const char	CLASSNAME[];
		static WgScrollbarPtr	Cast( const WgObjectPtr& pObject );


		//____ Enums ____________________________________________

		enum BtnLayout
		{
			NONE		= 0,												///< No buttons in scrollbar at all.
			HEADER_FWD	= 1,												///< Forward button at the top/left end of scrollbar.
			HEADER_BWD	= 2,												///< Backward button at the top/left end of scrollbar.
			FOOTER_FWD	= 4,												///< Forward button at the bottom/right end of scrollbar.
			FOOTER_BWD	= 8,												///< Backward button at the bottom/right end of scrollbar.
			WINDOWS		= HEADER_BWD | FOOTER_FWD,							///< Like Microsoft Windows. (= HEADER_BWD|FOOTER_FWD)
			NEXT_VERT	= FOOTER_FWD | FOOTER_BWD,							///< Like NeXT's vertical dragbar. (= FOOTER_FWD|FOOTER_BWD)
			NEXT_HORR	= HEADER_BWD | HEADER_FWD,							///< Like NeXT's horizontal dragbar. (= HEADER_FWD|HEADER_BWD)
			ALL			= HEADER_FWD | FOOTER_FWD | HEADER_BWD | FOOTER_BWD, ///< Both forward and backward buttons in both ends.


			DEFAULT		= WINDOWS
		};

		enum BgPressMode
		{
			JUMP_PAGE,				///< Jump a page forward/backward when pressing scrollbar background.
			GOTO_POS				///< Move slider to the position pressed when pressing scrollbar background.
		};

		//____ Methods __________________________________________

		inline float			HandlePos() const;						///< @brief Get position of the scrollbar handle.
		inline float			HandleSize() const;						///< @brief Get the size of the scrollbar handle.
		inline WgSkinPtr 		BackgroundSkin() const;					///< @brief Get the skin used as a background for the handle slide area.
		inline WgSkinPtr 		HandleSkin() const;						///< @brief Get the skin used for the scrollbar handle.
		inline WgSkinPtr 		BwdButtonSkin() const;					///< @brief Get the skin used for the backward button.
		inline WgSkinPtr 		FwdButtonSkin() const;					///< @brief Get the skin used for the forward button.
		inline BtnLayout		ButtonLayout() const;					///< @brief Get the layout of the forward/backward buttons.

		void			SetOrientation( WgOrientation orientation );	///< @brief Set scrollbar to vertical or horizontal.
		inline WgOrientation	Orientation() const; 					///< @brief Check if scrollbar is vertical or horizontal.

		void		SetBackgroundPressMode( BgPressMode mode );			///< @brief Set action for mouse press on scrollbar background.
		inline BgPressMode BackgroundPressMode() const;				///< @brief Get action for mouse press on scrollbar background.

		void	SetHandle( float pos, float size );						///< @brief Set relative size and position of scrollbar handle.
		void	SetHandlePos( float pos );								///< @brief Set relative position of scrollbar handle.
		void	SetHandlePixelPos( int pos );								///< @brief Set pixel position of scrollbar handle.
		void	SetHandleSize( float size );							///< @brief Set size of scrollbar handle.

		void	SetBackgroundSkin( const WgSkinPtr& pSkin );			///< @brief Set skin for background of handle slide area.
		void	SetHandleSkin( const WgSkinPtr& pSkin );				///< @brief Set skin for scrollbar handle.
		void	SetBwdButtonSkin( const WgSkinPtr& pSkin );				///< @brief Set skin for forward button.
		void	SetFwdButtonSkin( const WgSkinPtr& pSkin );				///< @brief Set skin for backward button.


		void	SetSkins( const WgSkinPtr& pBackgroundSkin, const WgSkinPtr& pHandleSkin,
						  const WgSkinPtr& pBwdButtonSkin, const WgSkinPtr& pFwdButtonSkin ); ///< @brief Set all skins in one go.
		void	SetButtonLayout(  BtnLayout layout );				///< @brief Set the layout for the forward/backward buttons.

		bool	SetScrollbarTarget( WgScrollbarTarget * pTarget );

		WgSize	PreferredSize() const;

	protected:
		WgScrollbar();
		virtual	~WgScrollbar();

		void	_onCloneContent( const WgWidget * _pOrg );
		void	_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip );
		void	_onEvent( const WgEventPtr& pEvent, WgEventHandler * pHandler );
		void	_onRefresh();
		bool	_onAlphaTest( const WgCoord& ofs, const WgSize& sz );
		void	_onStateChanged( WgState oldState );

		bool	_setHandle( float pos, float size );		// Set scrollbar pos/size without notifying target (but should post events).

		virtual WgWidget* _newOfMyType() const { return new WgScrollbar(); };

		enum Component
		{
			C_HEADER_FWD	= 0,
			C_HEADER_BWD	= 1,
			C_FOOTER_FWD	= 2,
			C_FOOTER_BWD	= 3,
			C_BG			= 4,
			C_HANDLE		= 5,

			C_NUMBER_OF_COMPONENTS	= 6,
			C_NONE			= -1,

		};

		WgSkinPtr		m_pBgSkin;
		WgSkinPtr		m_pHandleSkin;
		WgSkinPtr		m_pBtnFwdSkin;
		WgSkinPtr		m_pBtnBwdSkin;

		float			m_handlePos;
		float			m_handleSize;

		BgPressMode		m_bgPressMode;
		bool			m_bHorizontal;
		bool			m_bPressOnHandle;
		int				m_handlePressOfs;
		WgCoord			m_lastCursorDownPos;

		BtnLayout		m_btnLayout;
		Uint8			m_headerLen;
		Uint8			m_footerLen;

		WgState			m_states[C_NUMBER_OF_COMPONENTS];

		WgSize			m_minSize;

		WgScrollbarTarget *m_pScrollbarTargetInterface;			// So we can access our target.
		WgWidgetWeakPtr	m_pScrollbarTargetWidget;				// So we can check if target has been deleted.


	private:



		void	_viewToPosLen( int * _wpPos, int * _wpLen );
		void	_updateMinSize( void );


		Component	_findMarkedComponent( WgCoord ofs );								// -1 = None.
		void		_renderButton( WgGfxDevice * pDevice, const WgRect& _clip, WgRect& _dest, const WgSkinPtr& pSkin, WgState state );
		bool		_markTestButton( WgCoord ofs, WgRect& _dest, const WgSkinPtr& pSkin, WgState state );
		bool		_markTestHandle( WgCoord ofs );
		void		_headerFooterChanged();
		void		_unhoverReqRender();
};


//______________________________________________________________________________
/**
 * Get the relative position of the scrollbar handle.
 *
 * The relative position is a value in the range 0.0-1.0 where 0.0
 * is depending on the orientation of the scrollbar is either the
 * leftmost or topmost position and 1.0 is the bottom/right positon.
 *
 * The size of the handle doesn't affect range of the position, 0.5 always
 * leaves the handle in the middle of the scrollbar area.
 *
 * @return The position of the scrollbar handle in the range 0.0-1.0.
 **/

float WgScrollbar::HandlePos() const
{
	return m_handlePos;
}

//______________________________________________________________________________
/**
 * Get the relative size of the scrollbar handle.
 *
 * The relative size is a value in the range 0.0001-1.0, representing
 * the scrollbars size in relation to the area it can move over. 1.0
 * is an immobile handle that covers the whole area.
 *
 * The minimum size of the scrollbar handle is in reality limited by its skin,
 * but that doesn't affect this value, which remains what it has been set to.
 *
 * @return A value in the range 0.0001-1.0 representing the specified size of the handle.
 **/

float WgScrollbar::HandleSize() const
{
	return m_handleSize;
}

//______________________________________________________________________________
/**
 * Get a pointer to the skin used as a background for the area of the handle.
 *
 * This handle area background skin is different from the standard widget skin you get
 * using Skin(). The standard widget skin covers the whole widget and includes the buttons
 * and handle area as its content.
 *
 * The background skin returned by this method only covers the handle area and includes
 * only the scrollbar handle itself as its content.
 *
 * @return Pointer to skin used as a background for the area of the handle or null.
 **/

WgSkinPtr WgScrollbar::BackgroundSkin() const
{
	return m_pBgSkin;
}

//______________________________________________________________________________
/**
 * Get a pointer to the skin used for the scrollbar handle.
 *
 * @return Pointer to the skin used for the scrollbar handle or null.
 **/

WgSkinPtr WgScrollbar::HandleSkin() const
{
	return m_pHandleSkin;
}

//______________________________________________________________________________
/**
 * Get a pointer to the skin used by the backward button.
 *
 * The backward button is the button for moving the slider to the left
 * in a horizontal scrollbar and up in a vertical scrollbar.
 *
 * @return Pointer to the skin used for the backward button or null.
 **/

WgSkinPtr WgScrollbar::BwdButtonSkin() const
{
	return m_pBtnBwdSkin;
}

//______________________________________________________________________________
/**
 * Get a pointer to the skin used by the forward button.
 *
 * The forward button is the button for moving the slider to the right
 * in a horizontal scrollbar and down in a vertical scrollbar.
 *
 * @return Pointer to the skin used for the forward button or null.
 **/

WgSkinPtr WgScrollbar::FwdButtonSkin() const
{
	return m_pBtnFwdSkin;
}

//______________________________________________________________________________
/**
 * Get the layout of the scrollbar buttons
 *
 * The scrollbar can have a total of four button, one forward
 * and back button in the header and the same in the footer.
 *
 * ButtonLayout is essentially a bitmask specifying which of the
 * buttons are displayed.
 *
 * @return enum/bitmask specifying what buttons are displayed.
 **/

WgScrollbar::BtnLayout WgScrollbar::ButtonLayout() const
{
	return m_btnLayout;
}

//______________________________________________________________________________
/**
 * Check if scrollbar is vertical or horizontal
 *
 *
 *
 * @return 	WG_HORIZONTAL if scrollbar is horizontal,
 * 		   	WG_VERTICAL if scrollbar is vertical.
 **/

WgOrientation WgScrollbar::Orientation() const
{
	return m_bHorizontal?WG_HORIZONTAL:WG_VERTICAL;
}

//______________________________________________________________________________
/**
 * Get action for mouse press on scrollbar background.
 *
 * There are two ways that the scrollbar can react on a
 * mouse button press on the slider background. Either it
 * jumps a page up or down depending on if you pressed the background above
 * or below the handle, or it can jump to the pressed postion, just like if
 * you dragged the handle there.
 *
 * Default action is to jump page.
 *
 * @return JUMP_PAGE or GOTO_POS.
 **/

WgScrollbar::BgPressMode WgScrollbar::BackgroundPressMode() const
{
	return m_bgPressMode;
}


#endif //	WG_SCROLLBAR_DOT_H
