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
#ifndef WG_ISCROLLBAR_DOT_H
#define WG_ISCROLLBAR_DOT_H
#pragma once

#include <wg_pointers.h>
#include <wg_skin.h>

namespace wg
{
	class CScrollbar;

	class IScrollbar;
	typedef	StrongInterfacePtr<IScrollbar>	IScrollbar_p;
	typedef	WeakInterfacePtr<IScrollbar>	IScrollbar_wp;

	//____ IScrollbar __________________________________________________________

	class IScrollbar : public Interface
	{
	public:
		/** @private */

		IScrollbar(CScrollbar* pComponent) : m_pComponent(pComponent) {}


		enum class ButtonLayout
		{
			None,													///< No buttons in scrollbar at all.
			Normal,													///< Backward-button before slider, Forward-button after slider.
			BeforeSlider,											///< Both buttons placed before slider.
			AfterSlider,											///< Both buttons placed after slider.
			Overload												///< Both buttons placed both before and after slider.
		};


		//.____ Appearance __________________________________________

		void			setSkins(Skin * pBackgroundSkin, Skin * pHandleSkin,
								 Skin * pBwdButtonSkin, Skin * pFwdButtonSkin); ///< @brief Set all skins in one go.


		void			setBackgroundSkin(Skin * pSkin);				///< @brief Set skin for background of handle slide area.
		Skin * 			backgroundSkin() const;							///< @brief Get the skin used as a background for the handle slide area.

		void			setHandleSkin(Skin * pSkin);					///< @brief Set skin for scrollbar handle.
		Skin * 			handleSkin() const;								///< @brief Get the skin used for the scrollbar handle.

		void			setFwdButtonSkin(Skin * pSkin);					///< @brief Set skin for backward button.
		Skin * 			fwdButtonSkin() const;							///< @brief Get the skin used for the forward button.

		void			setBwdButtonSkin(Skin * pSkin);					///< @brief Set skin for forward button.
		Skin * 			bwdButtonSkin() const;							///< @brief Get the skin used for the backward button.

		void			setButtonLayout(ButtonLayout layout);			///< @brief Set the layout for the forward/backward buttons.
		ButtonLayout	buttonLayout() const;							///< @brief Get the layout of the forward/backward buttons.

		//.____ Behavior ____________________________________________

		void			setJumpToPress(bool bJumpToPress);				///< @brief Set action for mouse press on scrollbar background.
		bool			jumpToPress() const;	///< @brief Get action for mouse press on scrollbar background.

		//.____ Control _____________________________________________

		void			setHandlePos(float pos);						///< @brief Set relative position of scrollbar handle.
		float			handlePos() const;								///< @brief Get position of the scrollbar handle.

		void			stepForward(int nSteps = 1);
		void			stepBackward(int nSteps = 1);

		void			jumpForward();
		void			jumpBackward();

		void			jumpToBeginning();
		void			jumpToEnd();

	protected:
		Object *		_object() const override;

		CScrollbar * m_pComponent;
	};
} // namespace wg

#endif //WG_ISCROLLBAR_DOT_H
