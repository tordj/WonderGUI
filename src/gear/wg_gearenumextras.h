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

#ifndef	WG_GEARENUMEXTRAS_DOT_H
#define WG_GEARENUMEXTRAS_DOT_H
#pragma once

#include <wg_geartypes.h>

namespace wg
{

//. startAutoSection
/*=========================================================================

				>>> START OF AUTO GENERATED SECTION <<<

				Any modifications here will be overwritten!

=========================================================================*/

	const static ErrorSeverity   ErrorSeverity_min   = ErrorSeverity::Warning;
	const static ErrorCode       ErrorCode_min       = ErrorCode::OutOfRange;

	const static ErrorSeverity   ErrorSeverity_max   = ErrorSeverity::Critical;
	const static ErrorCode       ErrorCode_max       = ErrorCode::Other;

	const static int             ErrorSeverity_size  = (int)ErrorSeverity::Critical + 1;
	const static int             ErrorCode_size      = (int)ErrorCode::Other + 1;

	const char * toString(ErrorSeverity);
	const char * toString(ErrorCode);

//=========================================================================
//. endAutoSection


} // namespace wg

#endif //WG_GEARENUMEXTRAS_DOT_H
