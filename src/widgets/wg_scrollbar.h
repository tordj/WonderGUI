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

namespace wg 
{
	
	
	class Surface;
	class ScrollbarTarget;
	class Menu;
	
	
	class Scrollbar;
	typedef	StrongPtr<Scrollbar,Widget_p>		Scrollbar_p;
	typedef	WeakPtr<Scrollbar,Widget_wp>		Scrollbar_wp;
	
	//____ Class: Scrollbar _____________________________________________________
	/**
	 * @brief Scrollbar widget.
	 *
	 * Scrollbar is a normal scrollbar widget with buttons for stepping forward/backward
	 * and a resizable handle that can be dragged to move the view.
	 *
	 * There can be a total of four buttons and a handle (slider) in the scrollbar. The buttons
	 * are optional and can be enabled individually. They consists of a forward and backward button on each side
	 * of the slider.
	 *
	 * The same scrollbar class is used for vertical and horizontal scrollbars with the orientation
	 * setting deciding if scrollbar is displayed vertically or horizontally.
	 *
	 * Skinning a scrollbar is a little more complex than a normal widget since it consists of five individual skins. The first skin
	 * is the normal widget skin which encloses all components of the scrollbar. The two next skins are the skins
	 * for the forward and backward buttons. The fourth skin is the "background skin" which fills the area between
	 * the buttons in the header and the footer (the area the handle can move over). The fifth and final skin is
	 * the skin for the dragbar handle.
	 *
	 **/
	
	class	Scrollbar : public Widget
	{
		friend class ScrollbarTarget;
		friend class Menu;
	
		public:
			static Scrollbar_p	create() { return Scrollbar_p(new Scrollbar()); }
	
			bool		isInstanceOf( const char * pClassName ) const;
			const char *className( void ) const;
			static const char	CLASSNAME[];
			static Scrollbar_p	cast( const Object_p& pObject );
	
	
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
	
			inline float			handlePos() const;					///< @brief Get position of the scrollbar handle.
			inline float			handleSize() const;					///< @brief Get the size of the scrollbar handle.
			inline Skin_p 			backgroundSkin() const;				///< @brief Get the skin used as a background for the handle slide area.
			inline Skin_p 			handleSkin() const;					///< @brief Get the skin used for the scrollbar handle.
			inline Skin_p 			bwdButtonSkin() const;				///< @brief Get the skin used for the backward button.
			inline Skin_p 			fwdButtonSkin() const;				///< @brief Get the skin used for the forward button.
			inline BtnLayout		buttonLayout() const;				///< @brief Get the layout of the forward/backward buttons.
	
			void			setOrientation( Orientation orientation );	///< @brief Set scrollbar to vertical or horizontal.
			inline Orientation	orientation() const; 					///< @brief Check if scrollbar is vertical or horizontal.
	
			void		setBackgroundPressMode( BgPressMode mode );		///< @brief Set action for mouse press on scrollbar background.
			inline BgPressMode backgroundPressMode() const;				///< @brief Get action for mouse press on scrollbar background.
	
			void	setHandle( float pos, float size );					///< @brief Set relative size and position of scrollbar handle.
			void	setHandlePos( float pos );							///< @brief Set relative position of scrollbar handle.
			void	setHandlePixelPos( int pos );						///< @brief Set pixel position of scrollbar handle.
			void	setHandleSize( float size );						///< @brief Set size of scrollbar handle.
	
			void	setBackgroundSkin( const Skin_p& pSkin );			///< @brief Set skin for background of handle slide area.
			void	setHandleSkin( const Skin_p& pSkin );				///< @brief Set skin for scrollbar handle.
			void	setBwdButtonSkin( const Skin_p& pSkin );			///< @brief Set skin for forward button.
			void	setFwdButtonSkin( const Skin_p& pSkin );			///< @brief Set skin for backward button.
	
	
			void	setSkins( const Skin_p& pBaseSkin, const Skin_p& pBackgroundSkin, const Skin_p& pHandleSkin,
							  const Skin_p& pBwdButtonSkin, const Skin_p& pFwdButtonSkin ); ///< @brief Set all skins in one go.
			void	setButtonLayout(  BtnLayout layout );				///< @brief Set the layout for the forward/backward buttons.
	
			bool	setScrollbarTarget( ScrollbarTarget * pTarget );
	
			Size	preferredSize() const;
	
		protected:
			Scrollbar();
			virtual	~Scrollbar();
	
			void	_cloneContent( const Widget * _pOrg );
			void	_render( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, const Rect& _clip );
			void	_receive( const Msg_p& pMsg );
			void	_refresh();
			bool	_alphaTest( const Coord& ofs );
			void	_setState( State state );
	
			bool	_setHandle( float pos, float size );		// Set scrollbar pos/size without notifying target (but should post messages).
	
			virtual Widget* _newOfMyType() const { return new Scrollbar(); };
	
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
	
			Skin_p		m_pBgSkin;
			Skin_p		m_pHandleSkin;
			Skin_p		m_pBtnFwdSkin;
			Skin_p		m_pBtnBwdSkin;
	
			float			m_handlePos;
			float			m_handleSize;
	
			BgPressMode		m_bgPressMode;
			bool			m_bHorizontal;
			bool			m_bPressOnHandle;
			int				m_handlePressOfs;
	
			BtnLayout		m_btnLayout;
			uint8_t			m_headerLen;
			uint8_t			m_footerLen;
	
			State			m_states[C_NUMBER_OF_COMPONENTS];
	
			Size			m_minSize;
	
			ScrollbarTarget *m_pScrollbarTargetInterface;			// So we can access our target.
			Widget_wp	m_pScrollbarTargetWidget;				// So we can check if target has been deleted.
	
	
		private:
	
	
	
			void	_viewToPosLen( int * _wpPos, int * _wpLen );
			void	_updateMinSize( void );
	
	
			Component	_findMarkedComponent( Coord ofs );								// -1 = None.
			void		_renderButton( GfxDevice * pDevice, const Rect& _clip, Rect& _dest, const Skin_p& pSkin, State state );
			bool		_markTestButton( Coord ofs, Rect& _dest, const Skin_p& pSkin, State state );
			bool		_markTestHandle( Coord ofs );
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
	
	float Scrollbar::handlePos() const
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
	
	float Scrollbar::handleSize() const
	{
		return m_handleSize;
	}
	
	//______________________________________________________________________________
	/**
	 * Get a pointer to the skin used as a background for the area of the handle.
	 *
	 * This handle area background skin is different from the standard widget skin you get
	 * using skin(). The standard widget skin covers the whole widget and includes the buttons
	 * and handle area as its content.
	 *
	 * The background skin returned by this method only covers the handle area and includes
	 * only the scrollbar handle itself as its content.
	 *
	 * @return Pointer to skin used as a background for the area of the handle or null.
	 **/
	
	Skin_p Scrollbar::backgroundSkin() const
	{
		return m_pBgSkin;
	}
	
	//______________________________________________________________________________
	/**
	 * Get a pointer to the skin used for the scrollbar handle.
	 *
	 * @return Pointer to the skin used for the scrollbar handle or null.
	 **/
	
	Skin_p Scrollbar::handleSkin() const
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
	
	Skin_p Scrollbar::bwdButtonSkin() const
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
	
	Skin_p Scrollbar::fwdButtonSkin() const
	{
		return m_pBtnFwdSkin;
	}
	
	//______________________________________________________________________________
	/**
	 * Get the layout of the scrollbar buttons
	 *
	 * The scrollbar can have a total of four buttons, one forward
	 * and backward button in the header and the same in the footer.
	 *
	 * ButtonLayout is essentially a bitmask specifying which of the
	 * buttons are displayed.
	 *
	 * @return enum/bitmask specifying what buttons are displayed.
	 **/
	
	Scrollbar::BtnLayout Scrollbar::buttonLayout() const
	{
		return m_btnLayout;
	}
	
	//______________________________________________________________________________
	/**
	 * Check if scrollbar is vertical or horizontal
	 *
	 *
	 *
	 * @return 	Orientation::Horizontal if scrollbar is horizontal,
	 * 		   	Orientation::Vertical if scrollbar is vertical.
	 **/
	
	Orientation Scrollbar::orientation() const
	{
		return m_bHorizontal?Orientation::Horizontal:Orientation::Vertical;
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
	
	Scrollbar::BgPressMode Scrollbar::backgroundPressMode() const
	{
		return m_bgPressMode;
	}
	
	

} // namespace wg
#endif //	WG_SCROLLBAR_DOT_H
