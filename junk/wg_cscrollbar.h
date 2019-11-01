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
#ifndef WG_CSCROLLBAR_DOT_H
#define WG_CSCROLLBAR_DOT_H
#pragma once


#include <wg_types.h>
#include <wg_geo.h>
#include <wg_skin.h>
#include <wg_component.h>


namespace wg
{
	class IScrollbar;
	class Msg;


	//____ CScrollbar _____________________________________________________________

	class CScrollbar : public Component /** @private */
	{

	public:
		CScrollbar(ComponentHolder * pHolder);
		virtual ~CScrollbar() {};

		//____ Enums ____________________________________________

		enum BtnLayout
		{
			NONE = 0,													///< No buttons in scrollbar at all.
			HEADER_FWD = 1,												///< Forward button at the top/left end of scrollbar.
			HEADER_BWD = 2,												///< Backward button at the top/left end of scrollbar.
			FOOTER_FWD = 4,												///< Forward button at the bottom/right end of scrollbar.
			FOOTER_BWD = 8,												///< Backward button at the bottom/right end of scrollbar.
			WINDOWS = HEADER_BWD | FOOTER_FWD,							///< Like Microsoft Windows. (= HEADER_BWD|FOOTER_FWD)
			FOOTER_BOTH = FOOTER_FWD | FOOTER_BWD,						///< Like NeXT's vertical dragbar. (= FOOTER_FWD|FOOTER_BWD)
			HEADER_BOTH = HEADER_BWD | HEADER_FWD,						///< Like NeXT's horizontal dragbar. (= HEADER_FWD|HEADER_BWD)
			ALL = HEADER_FWD | FOOTER_FWD | HEADER_BWD | FOOTER_BWD,	///< Both forward and backward buttons in both ends.


			DEFAULT = WINDOWS
		};

		//.____ Misc ____________________________________________

		void				receive(Msg * pMsg, const RectI& canvas, int markOpacity);

		void				render(GfxDevice * pDevice, const RectI& _canvas);
		bool				alphaTest(const CoordI& ofs, SizeI size, int markOpacity );

		void				setEnabled(bool bEnabled);

		//.____ Geometry ____________________________________________

		void				setOrientation(Orientation orientation);		///< @brief Set scrollbar to vertical or horizontal.
		inline Orientation	orientation() const { return m_bHorizontal ? Orientation::Horizontal : Orientation::Vertical; }	///< @brief Check if scrollbar is vertical or horizontal.

		SizeI				preferredSize() const;

		//.____ Appearance __________________________________________

		void				setSkins( Skin * pBackgroundSkin, Skin * pHandleSkin,
									  Skin * pBwdButtonSkin, Skin * pFwdButtonSkin); ///< @brief Set all skins in one go.


		void				setBackgroundSkin(Skin * pSkin);				///< @brief Set skin for background of handle slide area.
		inline Skin * 		backgroundSkin() const { return m_pBgSkin; }	///< @brief Get the skin used as a background for the handle slide area.

		void				setHandleSkin(Skin * pSkin);					///< @brief Set skin for scrollbar handle.
		inline Skin * 		handleSkin() const { return m_pHandleSkin; }	///< @brief Get the skin used for the scrollbar handle.

		void				setFwdButtonSkin(Skin * pSkin);					///< @brief Set skin for backward button.
		inline Skin * 		fwdButtonSkin() const { return m_pBtnFwdSkin; }	///< @brief Get the skin used for the forward button.

		void				setBwdButtonSkin(Skin * pSkin);					///< @brief Set skin for forward button.
		inline Skin * 		bwdButtonSkin() const { return m_pBtnBwdSkin; }	///< @brief Get the skin used for the backward button.

		void				setButtonLayout(BtnLayout layout);				///< @brief Set the layout for the forward/backward buttons.
		inline BtnLayout	buttonLayout() const { return m_btnLayout; }	///< @brief Get the layout of the forward/backward buttons.

		//.____ Behavior ____________________________________________

		void				setJumpToPress(bool bJumpToPress);				///< @brief Set action for mouse press on scrollbar background.
		inline bool			jumpToPress() const { return m_bJumpToPress; }	///< @brief Get action for mouse press on scrollbar background.

		//.____ Control ____________________________________________

		void				setHandle(float pos, float size);				///< @brief Set relative size and position of scrollbar handle.
		void				setHandlePos(float pos);						///< @brief Set relative position of scrollbar handle.
		inline float		handlePos() const { return m_handlePos; }		///< @brief Get position of the scrollbar handle.

		void				setHandlePixelPos(int pos, SizeI size);			///< @brief Set pixel position of scrollbar handle.

		void				setHandleSize(float size);						///< @brief Set size of scrollbar handle.
		inline float		handleSize() const { return m_handleSize; }		///< @brief Get the size of the scrollbar handle.



	protected:
		enum Part
		{
			C_HEADER_FWD = 0,
			C_HEADER_BWD = 1,
			C_FOOTER_FWD = 2,
			C_FOOTER_BWD = 3,
			C_BG = 4,
			C_HANDLE = 5,

			C_NUMBER_OF_PARTS = 6,
			C_NONE = -1,

		};

		void	_updateMinSize(void);
		void	_viewToPosLen(int * _wpPos, int * _wpLen, SizeI size);

		void	_headerFooterChanged();
		bool	_markTestButton(CoordI ofs, RectI& _dest, Skin * pSkin, State state, int markOpacity);
		bool	_markTestHandle(CoordI ofs, SizeI& componentSize, int markOpacity);

		void	_renderButton(GfxDevice * pDevice, RectI& _dest, Skin * pSkin, State state);

		Part	_findMarkedPart(CoordI ofs, SizeI size, int markOpacity);
		
		void	_unhoverReqRender();



		Skin_p			m_pBgSkin;
		Skin_p			m_pHandleSkin;
		Skin_p			m_pBtnFwdSkin;
		Skin_p			m_pBtnBwdSkin;

		float			m_handlePos						= 0.f;
		float			m_handleSize					= 1.f;

		bool			m_bJumpToPress					= false;
		bool			m_bHorizontal					= true;
		bool			m_bPressOnHandle				= false;
		int				m_handlePressOfs				= 0;

		BtnLayout		m_btnLayout						= DEFAULT;
		uint16_t		m_headerLen						= 0;
		uint16_t		m_footerLen						= 0;

		State			m_states[C_NUMBER_OF_PARTS];

		SizeI			m_minSize;
	};
}

#endif //WG_CSCROLLBAR_DOT_H

