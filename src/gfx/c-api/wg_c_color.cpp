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

#include <type_traits>

#include <wg_c_color.h>
#include <wg_color.h>


static_assert( sizeof(wg_color) == sizeof(wg::HiColor), "wg_color differs in size from wg::HiColor!" );
static_assert( std::is_trivially_copyable<wg::HiColor>::value, "HiColor is not trivially copyable!" );

