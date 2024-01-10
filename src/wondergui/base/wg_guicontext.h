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

#ifndef	WG_GUICONTEXT_DOT_H
#define	WG_GUICONTEXT_DOT_H
#pragma once

#include <wg_gfxbase.h>

#include <assert.h>

#include <wg_types.h>

namespace wg
{
	class Font;
	class MsgRouter;
	class NumberLayout;
	class InputHandler;
	class TextLayout;
	class PackLayout;
	class Caret;
	class TextStyle;
	class Receiver;

	typedef	StrongPtr<MsgRouter>		MsgRouter_p;
	typedef	StrongPtr<NumberLayout>		NumberLayout_p;
	typedef	StrongPtr<InputHandler>		InputHandler_p;
	typedef	StrongPtr<TextLayout>		TextLayout_p;
	typedef	StrongPtr<Caret>			Caret_p;
	typedef	StrongPtr<TextStyle>		TextStyle_p;
	typedef	StrongPtr<PackLayout>		PackLayout_p;

	class GUIContext;
	typedef	StrongPtr<GUIContext>	GUIContext_p;
	typedef	WeakPtr<GUIContext>		GUIContext_wp;

	class GUIContext : public GfxContext
	{
		friend class Object;


	public:

		//.____ Creation __________________________________________

		static GUIContext_p	create() { return GUIContext_p(new GUIContext()); };

		//.____ Identification __________________________________________

		const TypeInfo&	typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Content ___________________________________________________________

		MsgRouter_p		msgRouter();
		InputHandler_p	inputHandler();

		void			setDefaultTextLayout( TextLayout * pTextLayout );
		TextLayout_p	defaultTextLayout();

		void			setDefaultCaret(Caret * pCaret);
		Caret_p			defaultCaret();

		void			setDefaultStyle( TextStyle * pStyle );
		TextStyle_p 	defaultStyle();

		void			setDefaultNumberLayout(NumberLayout * pFormatter);
		NumberLayout_p	defaultNumberLayout();

		void			setDefaultPackLayout( PackLayout * pPackLayout );
		PackLayout_p	defaultPackLayout();
		
		//.____ Internal ____________________________________________________________

		TextStyle*	_defaultStyle() { return m_pDefaultStyle; }



	private:
		GUIContext();
		virtual ~GUIContext();

		
		MsgRouter_p		m_pMsgRouter;
		InputHandler_p	m_pInputHandler;
		TextLayout_p	m_pDefaultTextLayout;
		Caret_p			m_pDefaultCaret;
		NumberLayout_p	m_pDefaultNumberLayout;
		TextStyle_p		m_pDefaultStyle;			// Duplicated in TextStyle itself for WG2 compatibility reasons.
		PackLayout_p	m_pDefaultPackLayout;

	};



} // namespace wg
#endif //WG_GUICONTEXT_DOT_H
