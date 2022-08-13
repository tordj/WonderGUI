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

#ifndef	WG_CABI_DOT_H
#define	WG_CABI_DOT_H
#pragma once

#include <wg_c_abi.h>

namespace wg
{
	class CABI
	{
	public:

		static bool	init(wg_c_calls_collection* pCallsCollection);

		static inline const wg_c_calls_collection* func() { return s_func; };

		static wg_c_calls_collection*		s_func;
	};




} // namespace wg
#endif //WG_CABI__DOT_H
