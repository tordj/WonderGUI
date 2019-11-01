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

#include <wg_iscrollbar.h>
#include <wg_cscrollbar.h>

#include <cassert>

namespace wg
{

	void IScrollbar::setSkins(Skin * pBackgroundSkin, Skin * pHandleSkin, Skin * pBwdButtonSkin, Skin * pFwdButtonSkin)
	{
		m_pComponent->setSkins(pBackgroundSkin, pHandleSkin, pBwdButtonSkin, pFwdButtonSkin);
	}


	void IScrollbar::setBackgroundSkin(Skin * pSkin)
	{
		m_pComponent->setBackgroundSkin(pSkin);
	}

	Skin * IScrollbar::backgroundSkin() const
	{
		return m_pComponent->backgroundSkin();
	}

	void IScrollbar::setHandleSkin(Skin * pSkin)
	{
		m_pComponent->setHandleSkin(pSkin);
	}

	Skin * IScrollbar::handleSkin() const
	{
		return m_pComponent->handleSkin();
	}

	void IScrollbar::setFwdButtonSkin(Skin * pSkin)
	{
		m_pComponent->setFwdButtonSkin(pSkin);
	}

	Skin * IScrollbar::fwdButtonSkin() const
	{
		return m_pComponent->fwdButtonSkin();
	}

	void IScrollbar::setBwdButtonSkin(Skin * pSkin)
	{
		m_pComponent->setFwdButtonSkin(pSkin);
	}

	Skin * IScrollbar::bwdButtonSkin() const
	{
		return m_pComponent->bwdButtonSkin();
	}

	void IScrollbar::setButtonLayout(ButtonLayout layout)
	{
		int internalLayout;

		switch (layout)
		{
			case ButtonLayout::None:
				internalLayout = CScrollbar::NONE;
				break;
			case ButtonLayout::Normal:
				internalLayout = CScrollbar::DEFAULT;
				break;
			case ButtonLayout::BeforeSlider:
				internalLayout = CScrollbar::HEADER_BOTH;
				break;
			case ButtonLayout::AfterSlider:
				internalLayout = CScrollbar::FOOTER_BOTH;
				break;
			case ButtonLayout::Overload:
				internalLayout = CScrollbar::ALL;
				break;
		}

		m_pComponent->setButtonLayout( (CScrollbar::BtnLayout) internalLayout);
	}

	IScrollbar::ButtonLayout IScrollbar::buttonLayout() const
	{
		int internalLayout = m_pComponent->buttonLayout();

		switch (internalLayout)
		{
			case CScrollbar::NONE:
				return ButtonLayout::None;

			case CScrollbar::DEFAULT:
				return ButtonLayout::Normal;

			case CScrollbar::HEADER_BOTH:
				return ButtonLayout::BeforeSlider;

			case CScrollbar::FOOTER_BOTH:
				return ButtonLayout::AfterSlider;

			case CScrollbar::ALL:
				return ButtonLayout::Overload;

			default:
				assert(false);
				return ButtonLayout::Normal;
		}
	}


	void IScrollbar::setJumpToPress(bool bJumpToPress)
	{
		m_pComponent->setJumpToPress(bJumpToPress);
	}

	bool IScrollbar::jumpToPress() const
	{
		return m_pComponent->jumpToPress();
	}

	void IScrollbar::setHandlePos(float pos)
	{
		m_pComponent->setHandlePos(pos);
		m_pComponent->_notify(ComponentNotif::ValueEdited, 0, nullptr );
	}


	float IScrollbar::handlePos() const
	{
		return m_pComponent->handlePos();
	}

	void IScrollbar::stepForward(int nSteps)
	{
		m_pComponent->_notify(ComponentNotif::StepForward, nSteps, nullptr);
	}

	void IScrollbar::stepBackward(int nSteps)
	{
		m_pComponent->_notify(ComponentNotif::StepBackward, nSteps, nullptr);
	}

	void IScrollbar::jumpForward()
	{
		m_pComponent->_notify(ComponentNotif::JumpForward, 0, nullptr);
	}

	void IScrollbar::jumpBackward()
	{
		m_pComponent->_notify(ComponentNotif::JumpBackward, 0, nullptr);
	}

	void IScrollbar::jumpToBeginning()
	{
		m_pComponent->setHandlePos(0.f);
		m_pComponent->_notify(ComponentNotif::ValueEdited, 0, nullptr);
	}

	void IScrollbar::jumpToEnd()
	{
		m_pComponent->setHandlePos(1.f);
		m_pComponent->_notify(ComponentNotif::ValueEdited, 0, nullptr);
	}



	Object * IScrollbar::_object() const
	{
		return m_pComponent->_object();
	}

} // namespace wg

