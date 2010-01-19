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

#ifndef	WG_RESOURCESERIALIZER_XML_DOT_H
#define	WG_RESOURCESERIALIZER_XML_DOT_H

#include <string>
#include <vector>
#include <map>
#include <stack>
#include <sstream>

#include <wg_textprop.h>

//#include <expat.h>
#include "../../expat/lib/expat.h"

class WgResDB;
class WgResourceXML;

class WgXmlAttribute
{
public:
	WgXmlAttribute() {}
	WgXmlAttribute(const std::string& name) : m_name(name) {}
	WgXmlAttribute(const std::string& name, const std::string& value) : m_name(name), m_value(value) {}

	const std::string&	GetName() const		{ return m_name; }
	const std::string&	GetValue() const	{ return m_value; }
	std::string&		GetValue()			{ return m_value; }

	static WgXmlAttribute& Empty() { return m_empty; }

private:
	void Tokenize(std::vector<std::string>& tokens) const;

	std::string m_name;
	std::string m_value;
	static WgXmlAttribute m_empty;
};

class WgXmlNode
{
public:

	WgXmlNode() {}
	WgXmlNode(const XML_Char* name);

	void					SetName(const std::string& name) { m_name = name; }
	const std::string&		GetName() const	{ return m_name; }

	Uint32					GetAttributeCount() const		{ return m_attributes.size(); }
	const WgXmlAttribute&	GetAttribute(int i) const		{ return m_attributes[i]; }
	bool					HasAttribute(const std::string& name) const;
	WgXmlAttribute&			AddAttribute(const std::string& name);
	WgXmlAttribute&			AddAttribute(const std::string& name, const std::string& value);
	bool					RemoveAttribute(const std::string& name);
	void					ClearAttributes() { m_attributes.clear(); }

	const std::string&		operator[](const std::string& name) const;
	std::string&			operator[](const std::string& name);

private:
	std::string					m_name;
	std::vector<WgXmlAttribute> m_attributes;
	static WgXmlNode			m_empty;
};

class WgResourceSerializerXML
{
public:
	WgResourceSerializerXML( WgResDB * pDB );
	~WgResourceSerializerXML();

	bool		Read( const char * pXML, int nBytes );
	std::stringstream&	Write( );

	bool				HasErrors() const				{ return m_Err.size() > 0; }
	const std::string&	GetError() const				{ return m_Err; }
	void				Error(const std::string& err)	{ m_Err += err + "\n"; }
	void				Warning(const std::string& warn);
	void				Error(const std::string& err, const char* pFile, int iLine);

	void				BeginTag(const std::string& tagName);
	void				BeginTag(const std::string& tagName, const WgXmlNode& oldNode);
	void				AddAttribute(const std::string& name, const std::string& value);
	void				AddAttribute(const WgXmlAttribute& attr);
	bool				RemoveAttribute(const std::string& name);
	void				AddText(const std::string& text);
	void				EndTag();

	WgResDB*			ResDb() const { return m_pDB; }

private:

	struct OutNode
	{
		OutNode() : parent(0) {}
		OutNode(const WgXmlNode& xmlNode, OutNode* parent) : parent(parent), xmlNode(xmlNode) {}
		~OutNode() { for(Uint32 i =0 ; i < children.size(); i++) delete children[i]; }

		OutNode*				parent;
		std::vector<OutNode*>	children;
		WgXmlNode				xmlNode;
		std::stringstream		text;
	};

	static void XMLCALL		cbStartXMLTag( void * pData, const XML_Char * pElement, const XML_Char ** pAttr );
	static void XMLCALL		cbEndXMLTag( void * pData, const XML_Char * pElement );
	static void XMLCALL		cbXMLText( void * pData, const XML_Char * pChars, int len);

	bool					BuildXMLTree( const char * pXML, int nBytes );

	void					StartXMLTag( const XML_Char * pElement, const XML_Char ** pAttr );
	void					EndXMLTag( const XML_Char * pElement );
	void					XMLText( const XML_Char * pChars, int len);

	void					WriteNode(std::stringstream& ss, const OutNode& node, int nDepth);

	std::vector<WgResourceXML*>	m_xmlStackIn;

	OutNode* m_outRoot;
	OutNode* m_current;
	std::stringstream m_outData;

	struct OutTag
	{
		OutTag() : full(false)  {}
		OutTag(const OutTag& o) { tagName = o.tagName; full = o.full; data << o.data.str(); }
		OutTag& operator=(const OutTag& o) { tagName = o.tagName; full = o.full; data.clear(); data << o.data.str(); return *this; }
		std::string tagName;
		std::stringstream data;
		bool full;
	};
	std::vector<OutTag>		m_xmlStackOut;
	WgResourceXML*			m_xmlRoot;
	WgResDB *				m_pDB;
	std::string				m_Err;
	std::stringstream		m_output;
	//---
};


#endif //WG_RESOURCESERIALIZER_XML_DOT_H
