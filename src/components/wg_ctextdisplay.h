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

#ifndef	WG_CTEXTDISPLAY_DOT_H
#define WG_CTEXTDISPLAY_DOT_H
#pragma once

#include <wg_types.h>
#include <wg_cstatictextdisplay.h>


#include <wg_base.h>

namespace wg
{

	class String;
	class CharSeq;
	class CharBuffer;

	class CTextDisplay;
	typedef	StrongComponentPtr<CTextDisplay>	CTextDisplay_p;
	typedef	WeakComponentPtr<CTextDisplay>		CTextDisplay_wp;


	//____ CTextDisplay __________________________________________________________________

	class CTextDisplay : public CStaticTextDisplay		/** @private */
	{
	public:

		CTextDisplay(Holder * pHolder) : CStaticTextDisplay(pHolder) {}

		//.____ Content _____________________________________________

		inline void			clear() { _clear(); }

		inline void			set(const CharSeq& seq) { _set(seq); }
		inline void			set(const CharBuffer * buffer) { _set(buffer); }
		inline void			set(const String& str) { _set(str); }

		inline int			append(const CharSeq& seq) { return _append(seq); }
		inline int			insert(int ofs, const CharSeq& seq) { return _insert(ofs, seq); }
		inline int			replace(int ofs, int nDelete, const CharSeq& seq) { return _replace(ofs, nDelete, seq); }
		inline int			erase(int ofs, int len) { return _erase(ofs, len); }

		//.____ Appearance _____________________________________________

		inline void			setCharStyle(TextStyle * pStyle) { _setCharStyle(pStyle); }
		inline void			setCharStyle(TextStyle * pStyle, int ofs, int len) { _setCharStyle(pStyle, ofs, len); }

		inline void			clearCharStyle() { _clearCharStyle(); }
		inline void			clearCharStyle(int ofs, int len) { _clearCharStyle(ofs, len); }

		//.____ Misc __________________________________________________

		inline CTextDisplay_p		ptr() { return CTextDisplay_p(this); }

	};


} // namespace wg
#endif //WG_CTEXTDISPLAY_DOT_H
