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

#include <wg_cgfxoutstream.h>
#include <assert.h>

namespace wg
{
	const TypeInfo CGfxOutStream::TYPEINFO = { "CGfxOutStream", &Component::TYPEINFO };

	//____ constructor ________________________________________________________

	CGfxOutStream::CGfxOutStream(CGfxOutStream::Holder * pHolder) :
		m_pHolder(pHolder)
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& CGfxOutStream::typeInfo(void) const
	{
		return TYPEINFO;
	}


} // namespace wg
