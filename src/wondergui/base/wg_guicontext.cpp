
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


#include <wg_guicontext.h>
#include <wg_base.h>

#include <wg_msgrouter.h>
#include <wg_basictextlayout.h>
#include <wg_basicnumberlayout.h>
#include <wg_inputhandler.h>

#include <wg_dummyfont.h>
#include <wg_memstack.h>
#include <wg_mempool.h>
#include <wg_textstyle.h>
#include <wg_texttool.h>
#include <wg_textstylemanager.h>
#include <wg_skinslotmanager.h>
#include <wg_bitmapcache.h>
#include <wg_packlayout.h>

#include <iostream>
#include <algorithm>

namespace wg
{
	const TypeInfo GUIContext::TYPEINFO = { "GUIContext", &GUIContext::TYPEINFO };


	//____ constructor __________________________________________________________

	GUIContext::GUIContext()
	{
		TextStyle::Blueprint textStyleBP;
		textStyleBP.font = DummyFont::create();

		m_pDefaultStyle = TextStyle::create( textStyleBP );

		m_pDefaultCaret = Caret::create();

		m_pDefaultTextLayout = BasicTextLayout::create({});

		m_pDefaultNumberLayout = BasicNumberLayout::create( BasicNumberLayout::Blueprint() );

		m_pDefaultPackLayout = PackLayout::create( WGBP(PackLayout,
														_.expandFactor = PackLayout::Factor::Weight,
														_.shrinkFactor = PackLayout::Factor::Weight
														));

		m_pMsgRouter = MsgRouter::create();
		m_pInputHandler = InputHandler::create();
	}

	//____ destructor ___________________________________________________________

	GUIContext::~GUIContext()
	{
		
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& GUIContext::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ msgRouter() _________________________________________________________

	MsgRouter_p	GUIContext::msgRouter()
	{
		return m_pMsgRouter;
	}

	//____ inputHandler() ______________________________________________________

	InputHandler_p GUIContext::inputHandler()
	{
		return m_pInputHandler;
	}

	//____ defaultCaret() ______________________________________________________

	Caret_p GUIContext::defaultCaret()
	{
		return m_pDefaultCaret;
	}

	//____ setDefaultCaret() ___________________________________________________

	void GUIContext::setDefaultCaret( Caret * pCaret )
	{
		m_pDefaultCaret = pCaret;
	}

	//_____ defaultTextLayout() ________________________________________________

	TextLayout_p GUIContext::defaultTextLayout()
	{
		return m_pDefaultTextLayout;
	}

	//____ setDefaultTextLayout() ___________________________________________________

	void GUIContext::setDefaultTextLayout( TextLayout * pTextLayout )
	{
		m_pDefaultTextLayout = pTextLayout;
	}

	//____ defaultNumberLayout() _____________________________________________

	NumberLayout_p GUIContext::defaultNumberLayout()
	{
		return m_pDefaultNumberLayout;
	}

	//____ setDefaultNumberLayout() _______________________________________________________

	void GUIContext::setDefaultNumberLayout(NumberLayout * pFormatter)
	{
		m_pDefaultNumberLayout = pFormatter;
	}

	//_____ defaultPackLayout() ________________________________________________

	PackLayout_p GUIContext::defaultPackLayout()
	{
		return m_pDefaultPackLayout;
	}

	//____ setDefaultPackLayout() ___________________________________________________

	void GUIContext::setDefaultPackLayout( PackLayout * pTextLayout )
	{
		m_pDefaultPackLayout = pTextLayout;
	}

	//____ defaultStyle() ______________________________________________________

	TextStyle_p GUIContext::defaultStyle()
	{
		return m_pDefaultStyle;
	}

	//____ setDefaultStyle() _______________________________________________________

	void GUIContext::setDefaultStyle( TextStyle * pStyle )
	{
		m_pDefaultStyle = pStyle;
		
		if( Base::context() == this )
			TextStyle::s_pDefaultStyle = pStyle;

	}


} // namespace wg
