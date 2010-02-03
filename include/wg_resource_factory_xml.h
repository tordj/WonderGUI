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


#ifndef WG_RESOURCE_FACTORY_XML_DOT_H
#define WG_RESOURCE_FACTORY_XML_DOT_H

#ifndef	WG_TYPES_DOT_H
#	include <wg_types.h>
#endif

#include <string>
#include <map>

class WgResDB;
class WgXmlNode;
class WgResourceFactoryXML
{
	typedef class WgResourceXML*(*CreateFunc)(WgResourceXML*);

public:

	static int				Register(const std::string &name, CreateFunc func);
	static bool				Contains(const std::string &name);
	static WgResourceXML*	Create(const std::string &name, WgResourceXML* parent);
	static void				Clear();

	template<typename T>
	struct AutoReg { static WgResourceXML* Create(WgResourceXML* parent) { return new T(parent); } };

	template<typename T>
	static int Register(const std::string &name)
	{
		return Register(name, AutoReg<T>::Create);
	}

private:
	typedef std::map<std::string, CreateFunc> MapType;
	static MapType m_Map;
};

#endif
