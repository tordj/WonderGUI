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

#ifndef	WG_CSTATICTEXTDISPLAY_DOT_H
#define WG_CSTATICTEXTDISPLAY_DOT_H
#pragma once

#include <wg_types.h>
#include <wg_ctext.h>


#include <wg_base.h>

namespace wg
{

	class String;
	class CharSeq;
	class CharBuffer;

	class CStaticTextDisplay;
	typedef	StrongComponentPtr<CStaticTextDisplay>		CStaticTextDisplay_p;
	typedef	WeakComponentPtr<CStaticTextDisplay>		CStaticTextDisplay_wp;


	//____ CStaticTextDisplay __________________________________________________________________

	class CStaticTextDisplay : public CText
	{
	public:

		//.____ Blueprint ______________________________________________________

		struct Blueprint
		{
			TextLayout_p	layout;
			TextStyle_p		style;
		};

		CStaticTextDisplay(Widget * pWidget );


		//.____ Identification _________________________________________________

		const TypeInfo& typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Content _____________________________________________

		inline 	String			text() const { return CText::_getString(); }

		inline int				length() const { return m_charBuffer.length(); }
		inline bool				isEmpty() const { return length() == 0 ? true : false; }

		//.____ Misc __________________________________________________

		inline CStaticTextDisplay_p		ptr() { return CStaticTextDisplay_p(this); }

		TextLink_p			markedLink() const;

		//.____ Internal ______________________________________________________

		void				_initFromBlueprint(const Blueprint& blueprint);


	protected:

		virtual void		_receive( Msg * pMsg );

	protected:
		TextLink_p			m_pMarkedLink;	// Character offset for beginning of marked or focused link
	};


} // namespace wg
#endif //WG_CSTATICTEXTDISPLAY_DOT_H
