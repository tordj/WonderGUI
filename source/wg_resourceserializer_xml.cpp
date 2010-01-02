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

#include <string.h>
#include <assert.h>

#include <wg_resourceserializer_xml.h>

#ifndef WG_RESDB_DOT_H
#	include <wg_resdb.h>
#endif

#ifndef WG_RESOURCE_FACTORY_XML_DOT_H
#	include <wg_resource_factory_xml.h>
#endif

#ifndef WG_RESOURCES_XML_DOT_H
#	include <wg_resources_xml.h>
#endif

#ifndef WG_TEXTTOOL_DOT_H
#	include <wg_texttool.h>
#endif

#ifndef WG_UTIL_DOT_H
#	include <wg_util.h>
#endif

#include <sstream>

// TODO
// Add proper support for xml tags mixed with plain text

//____ () _________________________________________________________

WgResourceSerializerXML::WgResourceSerializerXML( WgResDB * pDB ) :
	m_xmlRoot(0)
{
	m_pDB = pDB;
	m_outRoot = 0;
}

//____ () _________________________________________________________

WgResourceSerializerXML::~WgResourceSerializerXML()
{
}

//____ () _________________________________________________________

bool WgResourceSerializerXML::Read( const char * pXML, int nBytes )
{
	WgResourceXML::RegisterResources();

	if(BuildXMLTree(pXML, nBytes) == false)
		return false;

	assert(m_xmlStackIn.size() == 0);

	return m_Err.size() == 0;
}

#include <windows.h>
void WgResourceSerializerXML::Error(const std::string& err, const char* pFile, int iLine)
{
	m_Err += err + "\n";
	std::stringstream ss;
	ss << pFile << "(" << iLine << "): " << err << std::endl;
	OutputDebugStringA(ss.str().c_str());
}

namespace
{
	inline std::string GetIndent(Uint32 nstack) { return std::string(2 * nstack, ' '); }
}

std::stringstream& WgResourceSerializerXML::Write( )
{
	m_outRoot = new OutNode();
	m_current = 0;
	WgXmlRoot root(0);
	root.Serialize(*this);
	assert(m_current == m_outRoot);
	m_outData.str(std::string());
	m_outData << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n\n";
	for(Uint32 iChild = 0; iChild < m_outRoot->children.size(); iChild++)
		WriteNode(m_outData, *m_outRoot->children[iChild], 0);
	delete m_outRoot;
	m_outRoot = 0;
	return m_outData;
}

void WgResourceSerializerXML::WriteNode(std::stringstream& ss, const OutNode& node, int nDepth)
{
	ss << GetIndent(nDepth) << "<" << node.xmlNode.GetName();

	for(Uint32 iAttr = 0; iAttr < node.xmlNode.GetAttributeCount(); iAttr++)
	{
		std::string escaped;
		WgUtil::XMLEscape(node.xmlNode.GetAttribute(iAttr).GetValue(), escaped);
		ss << " " << node.xmlNode.GetAttribute(iAttr).GetName() << "=\"" << escaped << "\"";
	}

	if( node.children.size() || node.text.str().size() )
	{
		ss << ">" << node.text.str();

		if(node.children.size())
		{
			ss << std::endl;
			WriteNode(ss, *node.children[0], nDepth + 1);
			for(Uint32 iChild = 1; iChild < node.children.size(); iChild++)
			{
				if(node.children[iChild - 1]->children.size() > 0 || node.children[iChild]->children.size() > 0)
					ss << std::endl;
				WriteNode(ss, *node.children[iChild], nDepth + 1);
			}
		}

		ss << GetIndent(nDepth) << "</" << node.xmlNode.GetName() << ">" << std::endl;
	}
	else
	{
		ss << "/>" << std::endl;
	}
}

void WgResourceSerializerXML::BeginTag(const std::string& tagName)
{
	BeginTag(tagName, tagName.c_str());
}

void WgResourceSerializerXML::BeginTag(const std::string& tagName, const WgXmlNode& oldNode)
{
	if(m_current == 0)
		m_current = m_outRoot;
	OutNode* node = new OutNode(oldNode, m_current);
	node->xmlNode.SetName(tagName);
	m_current->children.push_back(node);
	m_current = node;
}

void WgResourceSerializerXML::AddAttribute(const std::string& name, const std::string& value)
{
	std::string escaped;
	WgUtil::XMLEscape(value, escaped);
	m_current->xmlNode.AddAttribute(name, escaped);
}

void WgResourceSerializerXML::AddAttribute(const WgXmlAttribute& attr)
{
	m_current->xmlNode.AddAttribute(attr.GetName()).GetValue() = attr.GetValue();
}

bool WgResourceSerializerXML::RemoveAttribute(const std::string& name)
{
	return m_current->xmlNode.RemoveAttribute(name);
}

void WgResourceSerializerXML::AddText(const std::string& text)
{
	m_current->text << text;
}

void WgResourceSerializerXML::EndTag()
{
	m_current = m_current->parent;
}

bool WgResourceSerializerXML::BuildXMLTree( const char * pXML, int nBytes )
{
	m_xmlStackIn.clear();

	XML_Parser	hParser;

	// create parser
	hParser = XML_ParserCreate( "UTF-8" );

	XML_SetUserData( hParser, this );
	XML_SetElementHandler( hParser, cbStartXMLTag, cbEndXMLTag );
	XML_SetCharacterDataHandler( hParser, cbXMLText );
	
	int iRet = XML_Parse( hParser, pXML, nBytes, 1 );

	if( iRet == XML_STATUS_ERROR )
	{
		XML_Error errCode = XML_GetErrorCode( hParser );
		int errLine = XML_GetCurrentLineNumber( hParser );
		int errColumn = XML_GetCurrentColumnNumber( hParser );

		std::stringstream ss;
		ss << "XML parser error " << (int)errCode << " (Ln " << errLine << " Col " << errColumn << "): " << XML_ErrorString(errCode);
		Error(ss.str());
	}

	XML_ParserFree( hParser );
	return iRet == XML_STATUS_OK;
}

//____ () _________________________________________________________

void XMLCALL WgResourceSerializerXML::cbStartXMLTag( void * pData, const XML_Char * pElement, const XML_Char ** pAttr )
{
	((WgResourceSerializerXML*)pData)->StartXMLTag( pElement, pAttr );
}

//____ () _________________________________________________________

void XMLCALL WgResourceSerializerXML::cbEndXMLTag( void * pData, const XML_Char * pElement )
{
	((WgResourceSerializerXML*)pData)->EndXMLTag( pElement );
}

//____ () _________________________________________________________

void XMLCALL WgResourceSerializerXML::cbXMLText( void * pData, const XML_Char * pChars, int len)
{
	((WgResourceSerializerXML*)pData)->XMLText( pChars, len );
}


//____ () _________________________________________________________

void WgResourceSerializerXML::StartXMLTag( const XML_Char * pElement, const XML_Char ** pAttr )
{
	WgResourceXML* parent = 0;
	if(m_xmlStackIn.size())
	{
		parent = m_xmlStackIn.back();
	}

	assert(WgResourceFactoryXML::Contains(pElement));
	WgResourceXML* resource = WgResourceFactoryXML::Create(pElement, parent);
	if(resource == 0)
	{
		Error("<" + std::string(pElement) + "> is not supported");
		return;
	}

	if(parent == 0)
	{
		assert(m_xmlRoot == 0);
		m_xmlRoot = resource;
	}

	m_xmlStackIn.push_back(resource);

	WgXmlNode node(pElement);

	if( pAttr )
	{
		while( *pAttr )
		{
			const XML_Char* name = *pAttr++;
			const XML_Char* value = *pAttr++;
			node.AddAttribute(name, value);
		}
	}

	resource->OnDeserialize(node, *this);
}

//____ () _________________________________________________________

void WgResourceSerializerXML::EndXMLTag( const XML_Char * pElement )
{
	assert( !m_xmlStackIn.empty() );
	if( m_xmlStackIn.empty() )
		return;

	m_xmlStackIn.back()->OnDeserialized(*this);
	delete m_xmlStackIn.back();
	m_xmlStackIn.pop_back();
}

//____ () _________________________________________________________

void WgResourceSerializerXML::XMLText( const XML_Char * pChars, int len )
{
	assert(m_xmlStackIn.size() != 0);
	if(m_xmlStackIn.size() == 0)
		return;

	// ignore space only
	int i = 0;
	for(; i < len; i++)
	{
		if(!isspace(pChars[i]))
			break;
	}
	if(i == len)
		return;

	WgTextHolderRes* textRes = WgResourceXML::Cast<WgTextHolderRes>(m_xmlStackIn.back());
	if(textRes == 0)
	{
		Error("text chunk detected at invalid location");
		return;
	}

	textRes->DeserializeText(pChars, len);
}

//////////////////////////////////////////////////////////////////////////
/// WgXmlAttribute ///////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
WgXmlAttribute WgXmlAttribute::m_empty;

//////////////////////////////////////////////////////////////////////////
/// WgXmlNode ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
WgXmlNode WgXmlNode::m_empty;

WgXmlNode::WgXmlNode(const XML_Char* name) :
	m_name(name)
{
}

bool WgXmlNode::HasAttribute(const std::string& name) const
{
	for(size_t i = 0; i < m_attributes.size(); i++)
	{
		if(m_attributes[i].GetName() == name)
			return true;
	}
	return false;
}

const std::string& WgXmlNode::operator[](const std::string& name) const
{
	for(size_t i = 0; i < m_attributes.size(); i++)
	{
		if(m_attributes[i].GetName() == name)
			return m_attributes[i].GetValue();
	}
	return WgXmlAttribute::Empty().GetValue();
}

std::string& WgXmlNode::operator[](const std::string& name)
{
	for(size_t i = 0; i < m_attributes.size(); i++)
	{
		if(m_attributes[i].GetName() == name)
			return m_attributes[i].GetValue();
	}
	assert(0);
	return WgXmlAttribute::Empty().GetValue();
}

WgXmlAttribute& WgXmlNode::AddAttribute(const std::string& name)
{
	for(size_t i = 0; i < m_attributes.size(); i++)
	{
		if(m_attributes[i].GetName() == name)
			return m_attributes[i];
	}
	m_attributes.push_back(WgXmlAttribute(name));
	return m_attributes.back();
}

WgXmlAttribute& WgXmlNode::AddAttribute(const std::string& name, const std::string& value)
{
	WgXmlAttribute& attribute = AddAttribute(name);
	attribute.GetValue() = value;
	return attribute;
}

bool WgXmlNode::RemoveAttribute(const std::string& name)
{
	for(size_t i = 0; i < m_attributes.size(); i++)
	{
		if(m_attributes[i].GetName() == name)
		{
			m_attributes.erase(m_attributes.begin() + i);
			return true;
		}
	}
	return false;
}

void WgXmlAttribute::Tokenize(std::vector<std::string>& tokens) const
{	
	int i0 = 0;
	int i1 = m_value.find(',');
	do
	{
		tokens.push_back(m_value.substr(i0, i1 - i0));
		i0 = i1 + 1;
		i1 = m_value.find(',', i0);
	}
	while(i0 > 0);
}
