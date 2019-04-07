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

#ifndef WG_PAYLOAD_DOT_H
#define	WG_PAYLOAD_DOT_H
#pragma once

#include <wg_pointers.h>

namespace wg
{
  class Payload;
  typedef StrongPtr<Payload>   Payload_p;
  typedef WeakPtr<Payload>   Payload_wp;

  class Payload : public Object
  {
  public:
	  static Payload_p	create() { return Payload_p(new Payload()); }
  protected:
	  Payload() {}
  };

} // namespace wg
#endif //WG_WEAKPTR_DOT_H
