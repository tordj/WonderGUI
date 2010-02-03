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

#include <wg_resource_factory_xml.h>
#include <assert.h>

WgResourceFactoryXML::MapType WgResourceFactoryXML::m_Map;

int WgResourceFactoryXML::Register(const std::string &name, CreateFunc func)
{
	assert(m_Map.find(name) == m_Map.end());
	m_Map[name] = func;
	return m_Map.size();
}

bool WgResourceFactoryXML::Contains(const std::string &name)
{
	return m_Map.find(name) != m_Map.end();
}

WgResourceXML* WgResourceFactoryXML::Create(const std::string &name, WgResourceXML* parent)
{
	MapType::iterator i = m_Map.find(name);
	if(i != m_Map.end())
		return (i->second)(parent);
	assert(0);
	return 0;
}

void WgResourceFactoryXML::Clear()
{
	m_Map.clear();
}