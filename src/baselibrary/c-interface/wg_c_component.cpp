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

#include <wg_c_component.h>
#include <wg_component.h>
#include <wg_object.h>

using namespace wg;

inline Component * getComp(wg_component component )
{
    return reinterpret_cast<Component*>(component);
}


const wg_typeInfo* wg_getComponentTypeInfo(wg_component component)
{
	const TypeInfo& ret = getComp(component)->typeInfo();
	return reinterpret_cast<const wg_typeInfo*>(&ret);
}

int wg_isComponentInstanceOf(wg_component component, const wg_typeInfo* pTypeInfo)
{
	return getComp(component)->isInstanceOf(reinterpret_cast<const TypeInfo&>(*pTypeInfo));
}

wg_obj wg_getComponentObject(wg_component component)
{
    return getComp(component)->object().rawPtr();
}

