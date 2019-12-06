
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

#include <wg_icolumnheader.h>
#include <wg_ccolumnheader.h>

namespace wg
{
	//____ Constructor() ___________________________________________________________

	IColumnHeader::IColumnHeader(CColumnHeader* pComponent) : m_pComponent(pComponent), icon(&pComponent->icon), arrow(&pComponent->arrow), label(&pComponent->label)
	{
	}

	//____ _object() _______________________________________________________________

	Object * IColumnHeader::_object()
	{
		return m_pComponent->GeoComponent::_object();
	}

	const Object * IColumnHeader::_object() const
	{
		return m_pComponent->GeoComponent::_object();
	}

} // namespace wg
