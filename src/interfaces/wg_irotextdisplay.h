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

#ifndef WG_IROTEXTDISPLAY_DOT_H
#define WG_IROTEXTDISPLAY_DOT_H
#pragma once

#include <wg_ctextdisplay.h>
#include <wg_interface.h>
#include <wg_pointers.h>
#include <wg_textstyle.h>

namespace wg
{

	class Color;
	class CharSeq;
	class String;
	class CharBuffer;

	class IROTextDisplay;
	typedef	StrongInterfacePtr<IROTextDisplay>	IROTextDisplay_p;
	typedef	WeakInterfacePtr<IROTextDisplay>		IROTextDisplay_wp;

	/**
	 * @brief Interface for read-only text with modifiable appearance
	 *
	 * Interface that provides read-only access to the text of a text component and methods
	 * to modify its appearance.
	 *
	 */

	class IROTextDisplay : public Interface
	{
	public:
		/** @private */

		IROTextDisplay( CTextDisplay * pComponent ) : m_pComponent(pComponent) {};

		//.____ Appearance _____________________________________________

		inline void				setStyle( TextStyle * pStyle ) { m_pComponent->setStyle(pStyle); }
		inline void				clearStyle() { m_pComponent->clearStyle(); }
		inline TextStyle_p		style() const { return m_pComponent->style(); }

		inline void				setTextMapper( TextMapper * pTextMapper ) { m_pComponent->setTextMapper(pTextMapper); }
		inline void				clearTextMapper() { m_pComponent->clearTextMapper(); }
		inline TextMapper_p		textMapper() const { return m_pComponent->textMapper(); }

		//.____ State _______________________________________________

		inline State			state() const { return m_pComponent->state(); }

		//.____ Content _____________________________________________

		inline 	String			get() const { return m_pComponent->getString(); }
		inline int				length() const { return m_pComponent->length(); }
		inline bool				isEmpty() const { return m_pComponent->isEmpty(); }

		//.____ Misc __________________________________________________

		inline IROTextDisplay_p			ptr() { return IROTextDisplay_p(this); }

	protected:
		Object *				_object() const;

		CTextDisplay * 			m_pComponent;
	};



} // namespace wg
#endif //WG_TEXT_DOT_H
