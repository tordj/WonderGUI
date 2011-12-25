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

#ifndef WG_RESOURCES_XML_DOT_H
#define WG_RESOURCES_XML_DOT_H

#ifndef	WG_RESDB_DOT_H
#	include <wg_resdb.h>
#endif

#ifndef	WG_TYPES_DOT_H
#	include <wg_types.h>
#endif

#ifndef	WG_TEXTPROP_DOT_H
#	include <wg_textprop.h>
#endif

#ifndef	WG_ANIM_DOT_H
#	include <wg_gfxanim.h>
#endif

#ifndef WG_WIDGET_DOT_H
#	include <wg_widget.h>
#endif

#ifndef WG_RESOURCE_VISITOR_DOT_H
#	include <wg_resource_visitor.h>
#endif

#ifndef WDG_BASECLASS_VIEW_DOT_H
#	include <wdg_listview.h>
#endif

#ifndef WDG_GRIDVIEW_DOT_H
#	include <wdg_gridview.h>
#endif

#ifndef WDG_TABLEVIEW_DOT_H
#	include <wdg_tableview.h>
#endif

#ifndef WDG_EDITTEXTVIEW_DOT_H
#	include <wdg_textview.h>
#endif

#ifndef WDG_TEXT_DOT_H
#	include <wdg_text.h>
#endif


#ifndef WDG_DRAGBARS_DOT_H
#	include <wdg_dragbars.h>
#endif
#ifndef	WG_RESOURCESERIALIZER_XML_DOT_H
#	include <wg_resourceserializer_xml.h>
#endif

#include <vector>

class WgWidget;
class WgXmlNode;
class WgXmlAttribute;
class WgResourceSerializerXML;
class WgAnim;
class WgGfxFrame;
class WgSkinManager;
class WgIconHolder;

//////////////////////////////////////////////////////////////////////////
/// WgXMLMetaData ////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class WgXMLMetaData : public WgResDB::MetaData
{
public:
	WgXMLMetaData(WgXmlNode node) : xmlNode(node) {}

	virtual MetaData* Clone() const { return new WgXMLMetaData(xmlNode); }

	std::string GetAttribute(const std::string& name) const;
	bool		SetAttribute(const std::string& name, const std::string& value);
	bool		HasAttribute(const std::string& name) const;

	WgXmlNode xmlNode;
};

//////////////////////////////////////////////////////////////////////////
/// WgResourceXML ////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class WgResourceXML
{
public:
	WgResourceXML(WgResourceXML* parent) :
		m_parent(parent)
	{
	}

	virtual ~WgResourceXML() {}

	virtual void	Accept(WgResourceVisitor* visitor) = 0;

	void 			OnDeserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s);
	void 			OnDeserialized(WgResourceSerializerXML& s);

	virtual void 	Serialize(WgResourceSerializerXML& s) = 0;
	virtual void 	Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s) = 0;
	virtual void 	Deserialized(WgResourceSerializerXML& s) {}

	void			SetMetaData(WgResDB::MetaData* metaData);

	WgResourceXML*			Parent()		{ return m_parent; }
	const WgResourceXML*	Parent() const	{ return m_parent; }
	const WgXmlNode&		XmlNode() const	{ return m_node; }


	static void		RegisterResources();
	static void		UnregisterResources();

	template<typename T>
	static T* Cast(WgResourceXML* res)
	{
		if(res == 0)
			return 0;
		CastVisitor<T> visitor;
		res->Accept(&visitor);
		return visitor.m_converted;
	}

protected:
	void			SetXmlNode(const WgXmlNode& node)		{ m_node = node; }

private:
	template<typename T>
	struct CastVisitor : public WgResourceVisitor
	{
		CastVisitor() : m_converted(0) {}
		virtual void Visit(T* converted) { m_converted = converted; }
		T* m_converted;
	};

	WgResourceXML*	m_parent;
	WgXmlNode		m_node;
};

//////////////////////////////////////////////////////////////////////////
/// WgXmlRoot ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class WgXmlRoot : public WgResourceXML
{
public:
	WgXmlRoot(WgResourceXML* parent) : WgResourceXML(parent) {}

	virtual void Serialize(WgResourceSerializerXML& s);
	virtual void Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s);

	virtual void Accept(WgResourceVisitor* visitor)	{ visitor->Visit(this); }

	static const char* TagName() { return "xml"; }
	static bool IsTopLevelWidget(WgWidget* widget, WgResourceSerializerXML& s);
};

//////////////////////////////////////////////////////////////////////////
/// WgConnectRes /////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class WgConnectRes : public WgResourceXML
{
public:
	class ConnectData : public WgXMLMetaData
	{
	public:
		ConnectData(WgXmlNode node) : WgXMLMetaData(node), pEmitter(0) {}
		ConnectData(WgWidget* pEmitter, WgXmlNode node) : WgXMLMetaData(node), pEmitter(pEmitter) {}

		WgWidget* pEmitter;
	};

	WgConnectRes(WgResourceXML* parent) : WgResourceXML(parent) {}

	static void Serialize(WgResourceSerializerXML& s, WgWidgetRes* widgetRes);
	virtual void Serialize(WgResourceSerializerXML& s);
	virtual void Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s);

	virtual void	Accept(WgResourceVisitor* visitor) {}

	static const char* TagName() { return "connect"; }
};

//////////////////////////////////////////////////////////////////////////
/// WgReferenceRes ///////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class WgReferenceRes : public WgResourceXML
{
public:
	WgReferenceRes(WgResourceXML* parent, WgResDB::ResDBRes* resDbRes = 0) : WgResourceXML(parent), m_pResDbRes(resDbRes) {}

	virtual void Serialize(WgResourceSerializerXML& s);
	virtual void Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s);

	virtual void	Accept(WgResourceVisitor* visitor) {}

	static const char* TagName() { return "include"; }
private:
	WgResDB::ResDBRes* m_pResDbRes;
};

//////////////////////////////////////////////////////////////////////////
/// WgInterceptRes ///////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class WgInterceptRes : public WgResourceXML
{
public:
	WgInterceptRes(WgResourceXML* parent) : WgResourceXML(parent) { }

	static void Serialize(WgResourceSerializerXML& s, WgWidget::ActionSource source, WgWidget::InterceptionRules rule );
	virtual void Serialize(WgResourceSerializerXML& s) {};	// Dummy
	virtual void Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s);

	virtual void Accept(WgResourceVisitor* visitor)	{ visitor->Visit(this); }

	static const char* TagName() { return "intercept"; }
};

//////////////////////////////////////////////////////////////////////////
/// WgPointerMaskRes /////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class WgPointerMaskRes
{
public:
	static void Serialize(WgResourceSerializerXML& s, const WgXmlNode& xmlNode, const std::string& attr, WgWidget::PointerMask mask, WgWidget::PointerMask def = WgWidget::POINTER_SOURCE_ALPHA);
	static WgWidget::PointerMask Deserialize(const WgXmlNode& xmlNode, const std::string& attr);
};

//////////////////////////////////////////////////////////////////////////
/// WgPointerStyleRes ////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class WgPointerStyleRes
{
public:
	static void Serialize(WgResourceSerializerXML& s, const WgXmlNode& xmlNode, const std::string& attr, WgPointerStyle style, WgPointerStyle def = WG_POINTER_DEFAULT);
	static WgPointerStyle Deserialize(const WgXmlNode& xmlNode, const std::string& attr);
};

//////////////////////////////////////////////////////////////////////////
/// WgModeRes ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class WgModeRes
{
public:
	static std::string Serialize(WgMode mode);
	static WgMode Deserialize(const std::string& value);
};

//////////////////////////////////////////////////////////////////////////
/// WgButtonLayoutRes ////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class WgButtonLayoutRes
{
public:
	static void Serialize(WgResourceSerializerXML& s, const WgXmlNode& xmlNode, const std::string& attr, WgGizmoDragbar::ButtonLayout layout, WgGizmoDragbar::ButtonLayout def = WgGizmoDragbar::DEFAULT);
	static WgGizmoDragbar::ButtonLayout Deserialize(const WgXmlNode& xmlNode, const std::string& attr);
};

//////////////////////////////////////////////////////////////////////////
/// WgModePropRes ////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class WgModePropRes : public WgResourceXML
{
public:
	WgModePropRes(WgResourceXML* parent, WgMode mode = WG_MODE_NORMAL) : WgResourceXML(parent), m_mode(mode) {}

	virtual void Serialize(WgResourceSerializerXML& s);
	virtual void Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s);

	virtual void Accept(WgResourceVisitor* visitor)	{ visitor->Visit(this); }

	static const char* TagName() { return "mode"; }
private:
	WgMode	m_mode;
};

//////////////////////////////////////////////////////////////////////////
/// WgTextPropRes ////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class WgTextPropRes : public WgResourceXML
{
public:
	WgTextPropRes(WgResourceXML* parent, WgTextPropPtr pProp = WgTextPropPtr() ) :
		WgResourceXML(parent),
		m_pProp(pProp),
		m_bgColor(0),
		m_bBgColor(false),
		m_color(0),
		m_bColored(false),
		m_style(WG_STYLE_NORMAL),
		m_underlined(false),
		m_size(0)
	{ }

	virtual void Serialize(WgResourceSerializerXML& s);
	virtual void Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s);
	virtual void Deserialized(WgResourceSerializerXML& s);

	virtual void Accept(WgResourceVisitor* visitor)	{ visitor->Visit(this); }

	WgTextProp*		GetTextProp()	{ return &m_prop; }

	WgColor			GetColor() const { return m_color; }
	WgColor			GetBgColor() const { return m_bgColor; }
	WgFontStyle		GetStyle() const { return m_style; }
	int				GetSize() const { return m_size; }
	bool			IsUnderlined() const { return m_underlined; }

	static const char* TagName() { return "textprop"; }

private:
	WgTextProp		m_prop;
	WgTextPropPtr	m_pProp;
	WgColor			m_bgColor;
//	std::string		m_bgColorName;
	bool			m_bBgColor;
	WgColor			m_color;
//	std::string		m_colorName;
	bool			m_bColored;
	WgFontStyle		m_style;
	bool			m_underlined;
	Uint32			m_size;
};

//////////////////////////////////////////////////////////////////////////
/// WgValueFormatRes /////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class WgValueFormatRes : public WgResourceXML
{
public:
	WgValueFormatRes(WgResourceXML* parent, const class WgValueFormat* format = 0) :
		WgResourceXML(parent),
		m_format(format)
	{ }

	virtual void Serialize(WgResourceSerializerXML& s);
	virtual void Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s);

	virtual void Accept(WgResourceVisitor* visitor)	{ }

	static const char* TagName() { return "valueformat"; }

private:
	const WgValueFormat* m_format;
};

//////////////////////////////////////////////////////////////////////////
/// WgValueHolderRes /////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class WgValueHolderRes
{
public:

	WgValueHolderRes() {}
	virtual ~WgValueHolderRes() {}

	void	Serialize(WgResourceXML* pThis, const WgXmlNode& xmlNode, WgResourceSerializerXML& s, class Wg_Interface_ValueHolder* holder);
	void	Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s, Wg_Interface_ValueHolder* holder);

	Wg_Interface_ValueHolder* GetHolder() { return m_holder; }

	virtual void Accept(WgResourceVisitor* visitor)	{ visitor->Visit(this); }
private:
	Wg_Interface_ValueHolder* m_holder;
};

//////////////////////////////////////////////////////////////////////////
/// WgFontStyleRes ///////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class WgFontStyleRes
{
public:
	static std::string Serialize(WgFontStyle style, WgResourceSerializerXML& s);
	static WgFontStyle Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s);
};

//////////////////////////////////////////////////////////////////////////
/// WgGeometryRes ////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class WgGeometryRes : public WgResourceXML
{
public:
	WgGeometryRes(WgResourceXML* parent) : WgResourceXML(parent)	{ }
	WgGeometryRes(WgResourceXML* parent, const WgXmlNode& node) : WgResourceXML(parent)	{ SetXmlNode(node); }

	virtual void Serialize(WgResourceSerializerXML& s);
	virtual void Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s);

	virtual void Accept(WgResourceVisitor* visitor)	{ visitor->Visit(this); }

	static const char* TagName() { return "geo"; }

private:
};

//////////////////////////////////////////////////////////////////////////
/// WgSizeRes ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class WgSizeRes
{
public:
	static void Serialize(WgResourceSerializerXML& s, const WgXmlNode& xmlNode, const std::string& attr, WgSize size, WgSize def);
	static WgSize Deserialize(const WgXmlNode& xmlNode, const std::string& attr, WgSize def = WgSize(0, 0));
};

//////////////////////////////////////////////////////////////////////////
/// WgRectRes ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class WgRectRes
{
public:

	static void Serialize(WgResourceSerializerXML& s, const WgXmlNode& xmlNode, const WgRect& rect);
	static WgRect Deserialize(const WgXmlNode& xmlNode);
	//static void Serialize(WgResourceSerializerXML& s, const WgXmlNode& xmlNode, const std::string& attr, const WgRect& rect, const WgRect& def = WgRect());
	//static WgRect Deserialize(const WgXmlNode& xmlNode, const std::string& attr, WgRect def = WgRect());
};

//////////////////////////////////////////////////////////////////////////
/// WgOrientationRes /////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class WgOrientationRes : public WgResourceXML
{
public:
	static void Serialize(WgResourceSerializerXML& s, const WgXmlNode& xmlNode, const std::string& attr, WgOrientation orientation, WgOrientation def = WG_NORTHWEST);
	static WgOrientation Deserialize(WgResourceSerializerXML& s, const std::string& value, WgOrientation def = WG_NORTHWEST);
};

//////////////////////////////////////////////////////////////////////////
/// WgBlockFlagsRes //////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class WgBlockFlagsRes : public WgResourceXML
{
public:
	static void Serialize(WgResourceSerializerXML& s, WgResourceXML* tag, const WgXmlNode& xmlNode, Uint32 flags);
	static Uint32 Deserialize(WgResourceSerializerXML& s, const WgXmlNode& xmlNode );
};



//////////////////////////////////////////////////////////////////////////
/// WgColorRes ///////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class WgColorRes : public WgResourceXML
{
public:
	WgColorRes(WgResourceXML* parent, WgColor col = WgColor::white) : WgResourceXML(parent), m_color(col) { }
	//WgColorRes(WgResourceXML* parent, const WgXmlAttribute& attr, WgColor col = WgColor::White()) : WgResourceXML(parent), m_color(col) { Deserialize(attr); }

	WgColor GetColor() const	{ return m_color; }
	operator WgColor() const	{ return m_color; }

	virtual void Serialize(WgResourceSerializerXML& s);
	virtual void Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s);

	static char* ToHexString(WgColor color);
	static void Serialize(WgResourceSerializerXML& s, const WgXmlNode& xmlNode, const std::string& attr, WgColor color, WgColor def = WgColor(0, 0xff));
	static WgColor Deserialize(WgResourceSerializerXML& s, const std::string& value, WgColor def = WgColor(0, 0xff));

	virtual void Accept(WgResourceVisitor* visitor)	{ visitor->Visit(this); }

	static const char* TagName() { return "color"; }
private:
	WgColor m_color;
};

//////////////////////////////////////////////////////////////////////////
/// WgSurfaceRes /////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class WgSurfaceRes : public WgResourceXML
{
public:
	WgSurfaceRes(WgResourceXML* parent, WgSurface* pSurface = 0) : WgResourceXML(parent), m_pSurf(pSurface) { }

	operator WgSurface*() { return m_pSurf; }

	virtual void Serialize(WgResourceSerializerXML& s);
	virtual void Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s);

	virtual void Accept(WgResourceVisitor* visitor)	{ visitor->Visit(this); }
	static const char* TagName() { return "surface"; }

private:
	WgSurface* m_pSurf;
};

//////////////////////////////////////////////////////////////////////////
/// WgGlyphSetRes ////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class WgGlyphSetRes : public WgResourceXML
{
public:
	WgGlyphSetRes(WgResourceXML* parent, WgGlyphSet* glyphSet = 0, int size = 0 )  : WgResourceXML(parent), m_pGlyphSet(glyphSet), m_bHasStyle(false), m_size(size) { }
	WgGlyphSetRes(WgResourceXML* parent, WgGlyphSet* glyphSet, WgFontStyle style, int size = 0 )  : WgResourceXML(parent), m_pGlyphSet(glyphSet), m_bHasStyle(true), m_style(style), m_size(size) { }


	virtual void Serialize(WgResourceSerializerXML& s);
	virtual void Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s);

	virtual void Accept(WgResourceVisitor* visitor)	{ visitor->Visit(this); }

	static const char* TagName() { return "glyphset"; }
private:
	class WgGlyphSet * m_pGlyphSet;
	bool		m_bHasStyle;
	WgFontStyle m_style;
	int			m_size;
};

//////////////////////////////////////////////////////////////////////////
/// WgFontRes ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class WgFontRes : public WgResourceXML
{
public:
	WgFontRes(WgResourceXML* parent, WgFont* font = 0) : WgResourceXML(parent), m_pFont(font) { }

	WgFont * GetFont() const { return m_pFont; }

	virtual void Serialize(WgResourceSerializerXML& s);
	virtual void Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s);

	virtual void Accept(WgResourceVisitor* visitor)	{ visitor->Visit(this); }

	static const char* TagName() { return "font"; }
private:
	WgFont *	m_pFont;
};

//////////////////////////////////////////////////////////////////////////
/// WgAnimRes ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class WgAnimRes : public WgResourceXML
{
public:
	WgAnimRes(WgResourceXML* parent, WgGfxAnim* anim = 0) : WgResourceXML(parent), m_pAnim(anim) { }

	WgGfxAnim* GetAnim() const { return m_pAnim; }

	virtual void Serialize(WgResourceSerializerXML& s);
	virtual void Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s);

	virtual void Accept(WgResourceVisitor* visitor)	{ visitor->Visit(this); }

	static std::string		FromPlayMode(WgAnimMode mode);
	static WgAnimMode	ToPlayMode(const std::string& playmode);

	static const char* TagName() { return "anim"; }
private:
	WgGfxAnim*	m_pAnim;
};

//////////////////////////////////////////////////////////////////////////
/// WgAnimFrameRes ///////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class WgKeyFrameRes : public WgResourceXML
{
public:
	WgKeyFrameRes(WgResourceXML* parent, WgGfxFrame* frame = 0, Uint32 duration = 0) : WgResourceXML(parent), m_pFrame(frame), m_duration(duration) { }

	WgGfxFrame* GetKeyFrame() const { return m_pFrame; }
	Uint32		GetDuration() const { return m_duration; }

	virtual void Serialize(WgResourceSerializerXML& s);
	virtual void Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s);

	virtual void	Accept(WgResourceVisitor* visitor) { }

	static const char* TagName() { return "frame"; }
private:
	WgGfxFrame*	m_pFrame;
	Uint32 m_duration;
};

//////////////////////////////////////////////////////////////////////////
/// WgCursorRes //////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class WgCursorRes : public WgResourceXML
{
public:
	WgCursorRes(WgResourceXML* parent, WgCursor* cursor = 0) : WgResourceXML(parent), m_pCursor(cursor) { }

	WgCursor* GetCursor() const { return m_pCursor; }

	virtual void Serialize(WgResourceSerializerXML& s);
	virtual void Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s);

	virtual void	Accept(WgResourceVisitor* visitor) { }

	static const char* TagName() { return "cursor"; }
private:

	std::string	BlitModeToString( WgCursor::BlitMode mode );
	WgCursor::BlitMode	StringToBlitMode( const std::string& str );

	WgCursor*	m_pCursor;
};

//////////////////////////////////////////////////////////////////////////
/// WgTextManagerRes /////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class WgTextManagerRes : public WgResourceXML
{
public:
	WgTextManagerRes(WgResourceXML* parent, WgTextManager* pTextManager = 0) : WgResourceXML(parent), m_pTextManager(pTextManager) { }

	WgTextManager* GetTextManager() const { return m_pTextManager; }

	virtual void Serialize(WgResourceSerializerXML& s);
	virtual void Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s);

	virtual void	Accept(WgResourceVisitor* visitor) { }

	static const char* TagName() { return "textmanager"; }
private:
	WgTextManager*	m_pTextManager;
};

//////////////////////////////////////////////////////////////////////////
/// WgSkinManagerRes /////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class WgSkinManagerRes : public WgResourceXML
{
public:
	WgSkinManagerRes(WgResourceXML* parent, WgSkinManager* pSkinManager = 0) : WgResourceXML(parent), m_pSkinManager(pSkinManager) { }

	WgSkinManager* GetSkinManager() const { return m_pSkinManager; }

	virtual void Serialize(WgResourceSerializerXML& s);
	virtual void Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s);

	virtual void	Accept(WgResourceVisitor* visitor) { }

	static const char* TagName() { return "skinmanager"; }
private:
	WgSkinManager*	m_pSkinManager;
};


//////////////////////////////////////////////////////////////////////////
/// WgBorderRes //////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class WgBorderRes : public WgResourceXML
{
public:
	WgBorderRes(WgResourceXML* parent, const WgBorders& borders = WgBorders()) : WgResourceXML(parent), m_Borders(borders) { }

	virtual void Serialize(WgResourceSerializerXML& s);
	virtual void Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s);

	static void Serialize(WgResourceSerializerXML& s, const WgXmlNode& xmlNode, const std::string& attr, const WgBorders& borders, const WgBorders& def = WgBorders(0));
	static WgBorders Deserialize(WgResourceSerializerXML& s, const std::string& value, const WgBorders& def = WgBorders(0));

	virtual void Accept(WgResourceVisitor* visitor)	{ visitor->Visit(this); }

	operator WgBorders() const { return m_Borders; }

	static const char* TagName() { return "border"; }
private:
	WgBorders m_Borders;
};

//////////////////////////////////////////////////////////////////////////
/// WgTileRes ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class WgTileRes : public WgResourceXML
{
public:
	enum TileFlag
	{
		Left = WG_TILE_LEFT,
		Right = WG_TILE_RIGHT,
		Top = WG_TILE_TOP,
		Bottom = WG_TILE_BOTTOM,
		Center = WG_TILE_CENTER,
		All = Left | Right | Top | Bottom | Center
	};

	WgTileRes(WgResourceXML* parent, Uint32 flags = 0) : WgResourceXML(parent), m_tile(flags) { }

	virtual void Serialize(WgResourceSerializerXML& s);
	virtual void Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s);

	virtual void Accept(WgResourceVisitor* visitor)	{ visitor->Visit(this); }

	Uint32	GetTileFlags() const { return m_tile; }

	static const char* TagName() { return "tile"; }
private:
	Uint32 m_tile;
};

//////////////////////////////////////////////////////////////////////////
/// WgBlockRes ///////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class WgBlockRes : public WgResourceXML
{
public:
	WgBlockRes(WgResourceXML* parent, WgMode mode = WG_MODE_NORMAL, int x = 0, int y = 0) : WgResourceXML(parent), m_mode(mode), m_x(x), m_y(y) { }

	virtual void Serialize(WgResourceSerializerXML& s);
	virtual void Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s);

	virtual void Accept(WgResourceVisitor* visitor)	{ visitor->Visit(this); }

	WgMode	GetMode() const		{ return m_mode; }
	int		GetX() const		{ return m_x; }
	int		GetY() const		{ return m_y; }

	static const char* TagName() { return "block"; }
private:
	WgMode m_mode;
	int m_x;
	int m_y;
};

//////////////////////////////////////////////////////////////////////////
/// WgLegoRes ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class WgLegoRes : public WgResourceXML
{
public:
	WgLegoRes(WgResourceXML* parent) : WgResourceXML(parent) {}

	virtual void Serialize(WgResourceSerializerXML& s);
	virtual void Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s);

	virtual void Accept(WgResourceVisitor* visitor)	{ }

	static const char* TagName() { return "lego"; }
};

//////////////////////////////////////////////////////////////////////////
/// WgColorSetRes ////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
typedef	WgSmartPtr<class WgColorSet> WgColorSetPtr;
class WgColorSetRes : public WgResourceXML
{
public:
	WgColorSetRes(WgResourceXML* parent, WgColorSetPtr pColorSet = 0) : WgResourceXML(parent), m_pColorSet(pColorSet) { }

	virtual void Serialize(WgResourceSerializerXML& s);
	virtual void Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s);

	virtual void Accept(WgResourceVisitor* visitor)	{ visitor->Visit(this); }

	WgColorSetPtr	GetColorSet() { return m_pColorSet; }

	static const char* TagName() { return "colorset"; }
private:
	WgColorSetPtr	m_pColorSet;
};


//////////////////////////////////////////////////////////////////////////
/// WgBlockSetRes ////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
typedef	WgSmartPtrPooled<class WgBlockSet> WgBlockSetPtr;
class WgBlockSetRes : public WgResourceXML
{
public:
	WgBlockSetRes(WgResourceXML* parent, WgBlockSetPtr block = 0) : WgResourceXML(parent), m_pBlockSet(block) { }

	virtual void Serialize(WgResourceSerializerXML& s);
	virtual void Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s);

	virtual void Accept(WgResourceVisitor* visitor)	{ visitor->Visit(this); }

	WgBlockSetPtr	GetBlockSet() { return m_pBlockSet; }

	static const char* TagName() { return "blockset"; }
private:
	WgRect	StateRect(const WgRect& src, int iState);
	int		StateFromRect(const WgRect& src, const WgRect& stateRect);

	WgBlockSetPtr	m_pBlockSet;
};

//////////////////////////////////////////////////////////////////////////
/// WgAltRes /////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class WgAltRes : public WgResourceXML
{
public:
	WgAltRes(WgResourceXML* parent) : WgResourceXML(parent) { }

	virtual void Serialize(WgResourceSerializerXML& s, int altNb );
	virtual void Serialize(WgResourceSerializerXML& s ) {}
	virtual void Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s);

	virtual void Accept(WgResourceVisitor* visitor)	{ visitor->Visit(this); }

	static const char* TagName() { return "alt"; }
private:
	WgRect	StateRect(const WgRect& src, int iState);
	int		StateFromRect(const WgRect& src, const WgRect& stateRect);

};

//////////////////////////////////////////////////////////////////////////
/// WgIconHolderRes //////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class WgIconHolderRes
{
public:

	WgIconHolderRes() {}
	virtual ~WgIconHolderRes() {}

	void	Serialize(WgResourceXML* pThis, const WgXmlNode& xmlNode, WgResourceSerializerXML& s, WgIconHolder* holder);
	void	Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s, WgIconHolder* holder);

};

//////////////////////////////////////////////////////////////////////////
/// WgTextHolderRes //////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class WgTextHolderRes
{
public:

	WgTextHolderRes() {}
	virtual ~WgTextHolderRes() {}

	virtual WgCharBuffer* GetCharBuffer() = 0;

	void	Serialize(WgResourceXML* pThis, const WgXmlNode& xmlNode, WgResourceSerializerXML& s, class Wg_Interface_TextHolder* holder);
	void	Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s, Wg_Interface_TextHolder* holder);
	virtual void DeserializeText(const char * pChars, int len);

	virtual void Accept(WgResourceVisitor* visitor)	{ visitor->Visit(this); }
};

//////////////////////////////////////////////////////////////////////////
/// WgEditTextRes ////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class WgEditTextRes : public WgTextHolderRes
{
public:

	WgEditTextRes() {}
	virtual ~WgEditTextRes() {}

	virtual WgCharBuffer* GetCharBuffer() = 0;

	void	Serialize(WgResourceXML* pThis, const WgXmlNode& xmlNode, WgResourceSerializerXML& s, class WgInterfaceEditText* holder);
	void	Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s, WgInterfaceEditText* holder);

	virtual void Accept(WgResourceVisitor* visitor)	{ WgTextHolderRes::Accept(visitor); }
};



//////////////////////////////////////////////////////////////////////////
/// WgPropRes ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class WgPropRes : public WgResourceXML, public WgTextHolderRes
{
public:
	WgPropRes(WgResourceXML* parent, WgTextPropPtr pDefaultProp = 0, WgTextPropPtr pProp = 0, const Uint16* text = 0) :
		WgResourceXML(parent),
		m_pDefaultProp(pDefaultProp),
		m_pProp(pProp),
		m_textOut(text)
	{ }

	virtual void Serialize(WgResourceSerializerXML& s);
	virtual void Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s);
	virtual void DeserializeText(const char * pChars, int len);
	virtual void Deserialized(WgResourceSerializerXML& s);
	virtual WgCharBuffer* GetCharBuffer();

	virtual void Accept(WgResourceVisitor* visitor)	{ visitor->Visit(this); WgTextHolderRes::Accept(visitor); }

	static const char* TagName() { return "prop"; }

private:
	WgTextPropPtr	m_pDefaultProp;
	WgTextPropPtr	m_pProp;
	WgTextProp		m_prop;
	const Uint16*	m_textOut;
	std::string		m_textIn;
};

class WgBoldTextRes : public WgResourceXML, public WgTextHolderRes
{
public:
	WgBoldTextRes(WgResourceXML* parent, WgCharBuffer* buffer = 0) : WgResourceXML(parent) { }

	virtual void Accept(WgResourceVisitor* visitor)	{ visitor->Visit(this); WgTextHolderRes::Accept(visitor); }

	virtual void Serialize(WgResourceSerializerXML& s);
	virtual void Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s);
	virtual void DeserializeText(const char * pChars, int len);
	virtual WgCharBuffer* GetCharBuffer();

	static const char* TagName() { return "b"; }
};

class WgItalicTextRes : public WgResourceXML, public WgTextHolderRes
{
public:
	WgItalicTextRes(WgResourceXML* parent) : WgResourceXML(parent) { }

	virtual void Accept(WgResourceVisitor* visitor)	{ visitor->Visit(this); WgTextHolderRes::Accept(visitor); }

	virtual void Serialize(WgResourceSerializerXML& s);
	virtual void Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s);
	virtual void DeserializeText(const char * pChars, int len);
	virtual WgCharBuffer* GetCharBuffer();

	static const char* TagName() { return "i"; }
};

class WgUnderlinedTextRes : public WgResourceXML, public WgTextHolderRes
{
public:
	WgUnderlinedTextRes(WgResourceXML* parent) : WgResourceXML(parent) { }

	virtual void Accept(WgResourceVisitor* visitor)	{ visitor->Visit(this); WgTextHolderRes::Accept(visitor); }

	virtual void Serialize(WgResourceSerializerXML& s);
	virtual void Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s);
	virtual void DeserializeText(const char * pChars, int len);
	virtual WgCharBuffer* GetCharBuffer();

	static const char* TagName() { return "u"; }
};

class WgBreakTextRes : public WgResourceXML, public WgTextHolderRes
{
public:
	WgBreakTextRes(WgResourceXML* parent) : WgResourceXML(parent) { }

	virtual void Accept(WgResourceVisitor* visitor)	{ visitor->Visit(this); WgTextHolderRes::Accept(visitor); }

	virtual void Serialize(WgResourceSerializerXML& s);
	virtual void Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s);
	virtual void DeserializeText(const char * pChars, int len);
	virtual WgCharBuffer* GetCharBuffer();

	static const char* TagName() { return "br"; }
};

//////////////////////////////////////////////////////////////////////////
/// WgWidgetRes //////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class WgWidgetRes : public WgResourceXML
{
public:
	WgWidget*		GetWidget() { return m_Widget; }
	void			SetWidget(WgWidget* widget) { m_Widget = widget; }	// dangerous. must be of correct type. TODO: validate

	virtual void	Serialize(WgResourceSerializerXML& s);
	virtual void	Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s);

	virtual void	Accept(WgResourceVisitor* visitor)	{ visitor->Visit(this); }

protected:

	WgWidgetRes(WgResourceXML* parent, WgWidget* widget = 0) : WgResourceXML(parent), m_Widget(widget) { }
	virtual ~WgWidgetRes();

	WgWidget*		m_Widget;
};

/// Wdg_Button_Res ///////////////////////////////////////////////////////
class Wdg_Button_Res : public WgWidgetRes, public WgTextHolderRes, public WgIconHolderRes
{
public:
	Wdg_Button_Res(WgResourceXML* parent, class Wdg_Button* widget = 0);
	virtual ~Wdg_Button_Res();

	class Wdg_Button*		GetWidget() { return (Wdg_Button*)WgWidgetRes::GetWidget(); }

	virtual WgCharBuffer*	GetCharBuffer();

	virtual void	Accept(WgResourceVisitor* visitor)	{ WgWidgetRes::Accept(visitor); WgTextHolderRes::Accept(visitor); }
	virtual void	Serialize(WgResourceSerializerXML& s);
	virtual void	Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s);
	static const char* TagName() { return "button"; }
private:
	WgCharBuffer* m_pCharBuffer;
};

/// Wdg_RefreshButton_Res ///////////////////////////////////////////////////////
class Wdg_RefreshButton_Res : public WgWidgetRes, public WgTextHolderRes, public WgIconHolderRes
{
public:
	Wdg_RefreshButton_Res(WgResourceXML* parent, class Wdg_RefreshButton* widget = 0);
	virtual ~Wdg_RefreshButton_Res();

	class Wdg_RefreshButton*		GetWidget() { return (Wdg_RefreshButton*)WgWidgetRes::GetWidget(); }

	virtual WgCharBuffer*	GetCharBuffer();

	virtual void	Accept(WgResourceVisitor* visitor)	{ WgWidgetRes::Accept(visitor); WgTextHolderRes::Accept(visitor); }
	virtual void	Serialize(WgResourceSerializerXML& s);
	virtual void	Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s);
	static const char* TagName() { return "refreshbutton"; }
private:
	WgCharBuffer* m_pCharBuffer;
};


/// Wdg_CheckBox2_Res ////////////////////////////////////////////////////
class Wdg_CheckBox2_Res : public WgWidgetRes, public WgTextHolderRes, public WgIconHolderRes
{
public:
	Wdg_CheckBox2_Res(WgResourceXML* parent, class Wdg_CheckBox2* widget = 0);
	virtual ~Wdg_CheckBox2_Res();

	class Wdg_CheckBox2*	GetWidget() { return (Wdg_CheckBox2*)WgWidgetRes::GetWidget(); }

	virtual WgCharBuffer*	GetCharBuffer();

	virtual void			Accept(WgResourceVisitor* visitor)	{ WgWidgetRes::Accept(visitor); WgTextHolderRes::Accept(visitor); }
	virtual void			Serialize(WgResourceSerializerXML& s);
	virtual void			Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s);

	static const char*		TagName() { return "checkbox"; }
private:
	WgCharBuffer* m_pCharBuffer;
};

/// Wdg_ComboBox_Res /////////////////////////////////////////////////////
class Wdg_ComboBox_Res : public WgWidgetRes, public WgEditTextRes
{
public:
	Wdg_ComboBox_Res(WgResourceXML* parent, class Wdg_ComboBox* widget = 0);
	virtual ~Wdg_ComboBox_Res();

	class Wdg_ComboBox*		GetWidget() { return (Wdg_ComboBox*)WgWidgetRes::GetWidget(); }

	virtual WgCharBuffer*	GetCharBuffer();

	virtual void			Accept(WgResourceVisitor* visitor)	{ WgWidgetRes::Accept(visitor); WgTextHolderRes::Accept(visitor); visitor->Visit(this); }
	virtual void			Serialize(WgResourceSerializerXML& s);
	virtual void			Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s);

	static const char*		TagName() { return "combobox"; }
private:
	WgCharBuffer* m_pCharBuffer;
};

/// Wdg_Container_Res ////////////////////////////////////////////////////
class Wdg_Container_Res : public WgWidgetRes
{
public:
	Wdg_Container_Res(WgResourceXML* parent, class Wdg_Container* widget = 0);

	class Wdg_Container*	GetWidget() { return (Wdg_Container*)WgWidgetRes::GetWidget(); }

	virtual void			Serialize(WgResourceSerializerXML& s);
	virtual void 			Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s);

	static const char*		TagName() { return "container"; }
};

/// Wdg_HDrag_Res ////////////////////////////////////////////////////////
class Wdg_HDrag_Res : public WgWidgetRes
{
public:
	Wdg_HDrag_Res(WgResourceXML* parent, class Wdg_HDrag* widget = 0);

	class Wdg_HDrag*		GetWidget() { return (Wdg_HDrag*)WgWidgetRes::GetWidget(); }

	virtual void 			Serialize(WgResourceSerializerXML& s);
	virtual void 			Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s);

	static const char*		TagName() { return "hdrag"; }
};

/// Wdg_VDrag_Res ////////////////////////////////////////////////////////
class Wdg_VDrag_Res : public WgWidgetRes
{
public:
	Wdg_VDrag_Res(WgResourceXML* parent, class Wdg_VDrag* widget = 0);

	class Wdg_VDrag*		GetWidget() { return (Wdg_VDrag*)WgWidgetRes::GetWidget(); }

	virtual void 			Serialize(WgResourceSerializerXML& s);
	virtual void 			Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s);
	static const char*		TagName() { return "vdrag"; }
};

class Wdg_EditValue_Res : public WgWidgetRes, public WgValueHolderRes
{
public:
	Wdg_EditValue_Res(WgResourceXML* parent, class Wdg_EditValue* widget = 0);

	class Wdg_EditValue*GetWidget() { return (Wdg_EditValue*)WgWidgetRes::GetWidget(); }

	virtual void		Accept(WgResourceVisitor* visitor)	{ WgWidgetRes::Accept(visitor); WgValueHolderRes::Accept(visitor); }

	virtual void			Serialize(WgResourceSerializerXML& s);
	virtual void			Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s);

	static const char*	TagName() { return "editvalue"; }
};

/// Wdg_Fill_Res /////////////////////////////////////////////////////////
class Wdg_Fill_Res : public WgWidgetRes
{
public:
	Wdg_Fill_Res(WgResourceXML* parent, class Wdg_Fill* widget = 0);

	class Wdg_Fill*		GetWidget() { return (Wdg_Fill*)WgWidgetRes::GetWidget(); }

	virtual void		Serialize(WgResourceSerializerXML& s);
	virtual void		Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s);

	static const char*	TagName() { return "fill"; }
};

/// WgItemHolderRes //////////////////////////////////////////////////////
class WgItemHolderRes
{
public:
	void	Serialize(WgResourceXML* pThis, const WgXmlNode& xmlNode, WgResourceSerializerXML& s, class Wg_Interface_ItemHolder* holder);
	void	Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s, class Wg_Interface_ItemHolder* holder);

	Wg_Interface_ItemHolder* GetHolder() const { return m_holder; }

	void	Accept(WgResourceVisitor* visitor)	{ visitor->Visit(this); }

private:
	Wg_Interface_ItemHolder* m_holder;
};

/// WgBaseViewRes ////////////////////////////////////////////////////////
class WgBaseViewRes
{
public:
	virtual class Wdg_Baseclass_View* GetView() = 0;

	void	Serialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s, class Wdg_Baseclass_View* view);
	void	Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s, Wdg_Baseclass_View* view);
	void	Accept(WgResourceVisitor* visitor)	{ visitor->Visit(this); }
};

/// Wdg_GridView_Res /////////////////////////////////////////////////////
class Wdg_GridView_Res : public WgWidgetRes, WgBaseViewRes, WgItemHolderRes
{
public:
	Wdg_GridView_Res(WgResourceXML* parent, class Wdg_GridView* widget = 0);

	virtual Wdg_Baseclass_View*	GetView() { return GetWidget(); }
	class Wdg_GridView*		GetWidget() { return (Wdg_GridView*)WgWidgetRes::GetWidget(); }

	virtual void			Accept(WgResourceVisitor* visitor)	{ WgWidgetRes::Accept(visitor); WgBaseViewRes::Accept(visitor); WgItemHolderRes::Accept(visitor); }

	virtual void			Serialize(WgResourceSerializerXML& s);
	virtual void			Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s);

	static const char*		TagName() { return "gridview"; }
};

/// Wdg_ListView_Res /////////////////////////////////////////////////////
class Wdg_ListView_Res : public WgWidgetRes, WgBaseViewRes, WgItemHolderRes
{
public:
	Wdg_ListView_Res(WgResourceXML* parent, class Wdg_ListView* widget = 0);

	virtual Wdg_Baseclass_View*	GetView() { return GetWidget(); }
	class Wdg_ListView*		GetWidget() { return (Wdg_ListView*)WgWidgetRes::GetWidget(); }

	virtual void			Accept(WgResourceVisitor* visitor)	{ WgWidgetRes::Accept(visitor); WgBaseViewRes::Accept(visitor); WgItemHolderRes::Accept(visitor); }
	virtual void			Serialize(WgResourceSerializerXML& s);
	virtual void			Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s);

	static const char*		TagName() { return "listview"; }
private:
	std::string		m_linemarkColName;
};

/// Wdg_Menu_Res /////////////////////////////////////////////////////////
class Wdg_Menu_Res : public WgWidgetRes
{
public:
	Wdg_Menu_Res(WgResourceXML* parent, class Wdg_Menu* widget = 0);

	class Wdg_Menu*		GetWidget() { return (Wdg_Menu*)WgWidgetRes::GetWidget(); }

	virtual void		Accept(WgResourceVisitor* visitor)	{ WgWidgetRes::Accept(visitor); visitor->Visit(this); }
	virtual void		Serialize(WgResourceSerializerXML& s);
	virtual void		Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s);

	static const char*	TagName() { return "menu"; }
};

/// Wdg_MenuBar_Res //////////////////////////////////////////////////////
class Wdg_MenuBar_Res : public WgWidgetRes
{
public:
	Wdg_MenuBar_Res(WgResourceXML* parent, class Wdg_MenuBar* widget = 0);

	class Wdg_MenuBar*	GetWidget() { return (Wdg_MenuBar*)WgWidgetRes::GetWidget(); }

	virtual void		Accept(WgResourceVisitor* visitor)	{ WgWidgetRes::Accept(visitor); visitor->Visit(this); }
	virtual void		Serialize(WgResourceSerializerXML& s);
	virtual void		Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s);

	static const char*	TagName() { return "menubar"; }
};

/// WgMenuBarItemRes //////////////////////////////////////////////////////
class WgMenuBarItemRes : public WgResourceXML
{
public:
	WgMenuBarItemRes(WgResourceXML* parent, class WgMenuBarItem* pBarItem = 0) :
		WgResourceXML(parent),
		m_pBarItem(pBarItem),
		m_pMenu(0),
		m_navKey(0)
	{}

	void				SetMenu(Wdg_Menu* pMenu) { m_pMenu = pMenu; }

	virtual void		Accept(WgResourceVisitor* visitor)	{ visitor->Visit(this); }
	virtual void		Serialize(WgResourceSerializerXML& s);
	virtual void		Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s);
	virtual void		Deserialized(WgResourceSerializerXML& s);

	static const char*	TagName() { return "menubaritem"; }
private:
	WgMenuBarItem* m_pBarItem;
	Wdg_Menu* m_pMenu;
	Uint16 m_navKey;
	std::string m_Title;
};

/// Wdg_Pixmap_Res ///////////////////////////////////////////////////////
class Wdg_Pixmap_Res : public WgWidgetRes
{
public:
	Wdg_Pixmap_Res(WgResourceXML* parent, class Wdg_Pixmap* widget = 0);

	class Wdg_Pixmap*	GetWidget() { return (Wdg_Pixmap*)WgWidgetRes::GetWidget(); }

	virtual void		Serialize(WgResourceSerializerXML& s);
	virtual void		Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s);

	static const char*	TagName() { return "pixmap"; }
};

/// Wdg_Lodder_Res ///////////////////////////////////////////////////////
class Wdg_Lodder_Res : public WgWidgetRes
{
public:
	Wdg_Lodder_Res(WgResourceXML* parent, class Wdg_Lodder* lodder = 0);

	class Wdg_Lodder*	GetWidget() { return (Wdg_Lodder*)WgWidgetRes::GetWidget(); }

	virtual void		Serialize(WgResourceSerializerXML& s);
	virtual void		Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s);
	virtual void 		Deserialized(WgResourceSerializerXML& s);

	static const char*	TagName() { return "lodder"; }
};

/// Wdg_RadioButton2_Res /////////////////////////////////////////////////
class Wdg_RadioButton2_Res : public WgWidgetRes, public WgTextHolderRes, public WgIconHolderRes
{
public:
	Wdg_RadioButton2_Res(WgResourceXML* parent, class Wdg_RadioButton2* widget = 0);
	virtual ~Wdg_RadioButton2_Res();

	class Wdg_RadioButton2*	GetWidget() { return (Wdg_RadioButton2*)WgWidgetRes::GetWidget(); }

	virtual WgCharBuffer*	GetCharBuffer();

	virtual void			Accept(WgResourceVisitor* visitor)	{ WgWidgetRes::Accept(visitor); WgTextHolderRes::Accept(visitor); }
	virtual void			Serialize(WgResourceSerializerXML& s);
	virtual void			Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s);

	static const char*		TagName() { return "radiobutton"; }
private:
	WgCharBuffer*	m_pCharBuffer;
};

/// Wdg_Root_Res /////////////////////////////////////////////////////////
class Wdg_Root_Res : public WgWidgetRes
{
public:
	Wdg_Root_Res(WgResourceXML* parent, class Wdg_Root* widget = 0);

	class Wdg_Root*		GetWidget() { return (Wdg_Root*)WgWidgetRes::GetWidget(); }

	virtual void		Serialize(WgResourceSerializerXML& s);
	virtual void		Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s);

	static const char*	TagName() { return "root"; }
};

/// Wdg_Shader_Res ///////////////////////////////////////////////////////
class Wdg_Shader_Res : public WgWidgetRes
{
public:
	Wdg_Shader_Res(WgResourceXML* parent, class Wdg_Shader* widget = 0);

	class Wdg_Shader*		GetWidget() { return (Wdg_Shader*)WgWidgetRes::GetWidget(); }

	virtual void	Serialize(WgResourceSerializerXML& s);
	virtual void	Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s);
	static const char* TagName() { return "shader"; }
private:
	std::string m_colorName;
};

/// WgTableColumnRes ////////////////////////////////////////////////////
class WgTableColumnRes : public WgResourceXML, WgTextHolderRes
{
public:
	WgTableColumnRes(WgResourceXML* parent, class WgTableColumn* column = 0) : WgResourceXML(parent), m_column(column), m_pCharBuffer(0) {}
	virtual ~WgTableColumnRes();

	virtual WgCharBuffer*	GetCharBuffer();

	virtual void	Accept(WgResourceVisitor* visitor)	{ WgTextHolderRes::Accept(visitor); }
	virtual void	Serialize(WgResourceSerializerXML& s);
	virtual void	Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s);
	static const char* TagName() { return "column"; }

private:
	WgTableColumn* m_column;
	WgCharBuffer* m_pCharBuffer;
};

/// Wdg_TableView_Res ////////////////////////////////////////////////////
class Wdg_TableView_Res : public WgWidgetRes, WgBaseViewRes, WgItemHolderRes
{
public:
	Wdg_TableView_Res(WgResourceXML* parent, class Wdg_TableView* widget = 0);

	virtual Wdg_Baseclass_View*	GetView() { return GetWidget(); }
	class Wdg_TableView*		GetWidget() { return (Wdg_TableView*)WgWidgetRes::GetWidget(); }

	virtual void	Accept(WgResourceVisitor* visitor)	{ WgWidgetRes::Accept(visitor); WgBaseViewRes::Accept(visitor); WgItemHolderRes::Accept(visitor); visitor->Visit(this); }
	virtual void	Serialize(WgResourceSerializerXML& s);
	virtual void	Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s);
	static const char* TagName() { return "tableview"; }
private:
	std::string		m_linemarkColName;
};

/// Wdg_TabList_Res //////////////////////////////////////////////////////
class Wdg_TabList_Res : public WgWidgetRes
{
public:
	Wdg_TabList_Res(WgResourceXML* parent, class Wdg_TabList* widget = 0);

	class Wdg_TabList*		GetWidget() { return (Wdg_TabList*)WgWidgetRes::GetWidget(); }

	virtual void	Accept(WgResourceVisitor* visitor)	{ WgWidgetRes::Accept(visitor); visitor->Visit(this); }

	virtual void	Serialize(WgResourceSerializerXML& s);
	virtual void	Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s);
	static const char* TagName() { return "tablist"; }
};

class WgTabRes : public WgResourceXML, public WgTextHolderRes
{
public:
	WgTabRes(WgResourceXML* parent, class WgTab* tab = 0);
	virtual ~WgTabRes();

	virtual WgCharBuffer*	GetCharBuffer();

	virtual void Serialize(WgResourceSerializerXML& s);
	virtual void Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s);

	void Serialize(WgXmlAttribute& attr) const;
	void Deserialize(const WgXmlAttribute& attr);

	virtual void Accept(WgResourceVisitor* visitor)	{ visitor->Visit(this); WgTextHolderRes::Accept(visitor); }

	void	SetTab(WgTab* tab) { m_tab = tab; }

	static const char* TagName() { return "tab"; }
private:
	WgTab* m_tab;
	WgCharBuffer* m_pCharBuffer;
};

/// Wdg_Text_Res /////////////////////////////////////////////////////
class Wdg_Text_Res : public WgWidgetRes, public WgEditTextRes
{
public:
	Wdg_Text_Res(WgResourceXML* parent, class Wdg_Text* widget = 0);
	virtual ~Wdg_Text_Res();

	class Wdg_Text*		GetWidget() { return (Wdg_Text*)WgWidgetRes::GetWidget(); }

	virtual WgCharBuffer*	GetCharBuffer();

	virtual void	Accept(WgResourceVisitor* visitor)	{ WgWidgetRes::Accept(visitor); WgEditTextRes::Accept(visitor); }
	virtual void	Serialize(WgResourceSerializerXML& s);
	virtual void	Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s);
	static const char* TagName() { return "text"; }
private:
	WgCharBuffer* m_pCharBuffer;
};

/// Wdg_EditLine_Res /////////////////////////////////////////////////////
class Wdg_EditLine_Res : public WgWidgetRes, public WgEditTextRes
{
public:
	Wdg_EditLine_Res(WgResourceXML* parent, class Wdg_Text* widget = 0);
	virtual ~Wdg_EditLine_Res();

	class Wdg_EditLine*		GetWidget() { return (Wdg_EditLine*)WgWidgetRes::GetWidget(); }

	virtual WgCharBuffer*	GetCharBuffer();

	virtual void	Accept(WgResourceVisitor* visitor)	{ WgWidgetRes::Accept(visitor); WgEditTextRes::Accept(visitor); }
	virtual void	Serialize(WgResourceSerializerXML& s);
	virtual void	Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s);
	static const char* TagName() { return "editline"; }
private:
	WgCharBuffer* m_pCharBuffer;
};

/// Wdg_TextView_Res /////////////////////////////////////////////////////
class Wdg_TextView_Res : public WgWidgetRes, WgBaseViewRes, WgEditTextRes
{
public:
	Wdg_TextView_Res(WgResourceXML* parent, class Wdg_TextView* widget = 0);
	virtual ~Wdg_TextView_Res();

	virtual Wdg_Baseclass_View*	GetView() { return GetWidget(); }
	class Wdg_TextView*	GetWidget() { return (Wdg_TextView*)WgWidgetRes::GetWidget(); }

	virtual WgCharBuffer*	GetCharBuffer();

	virtual void			Accept(WgResourceVisitor* visitor)	{ WgWidgetRes::Accept(visitor); WgBaseViewRes::Accept(visitor); WgEditTextRes::Accept(visitor); }
	virtual void			Serialize(WgResourceSerializerXML& s);
	virtual void			Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s);

	static const char*		TagName() { return "textview"; }
private:
	WgCharBuffer* m_pCharBuffer;
};

//////////////////////////////////////////////////////////////////////////
class Wdg_YSplitter_Res : public WgWidgetRes
{
public:
	Wdg_YSplitter_Res(WgResourceXML* parent, class Wdg_YSplitter* widget = 0);

	class Wdg_YSplitter*	GetWidget() { return (Wdg_YSplitter*)WgWidgetRes::GetWidget(); }

	virtual void			Accept(WgResourceVisitor* visitor)	{ WgWidgetRes::Accept(visitor); }
	virtual void			Serialize(WgResourceSerializerXML& s);
	virtual void			Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s);

	static const char*		TagName() { return "ysplitter"; }
};

//////////////////////////////////////////////////////////////////////////
/// WgItemRes ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class WgItemRes : public WgResourceXML
{
public:

	class WgItem*	GetItem()				{ return m_item; }
	void			SetItem(WgItem* item)	{ m_item = item; }

	virtual void	Serialize(WgResourceSerializerXML& s);
	virtual void	Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s);
	virtual void 	Deserialized(WgResourceSerializerXML& s);

	virtual void	Accept(WgResourceVisitor* visitor)	{ visitor->Visit(this); }

protected:
	WgItemRes(WgResourceXML* parent, WgItem* item = 0) : WgResourceXML(parent), m_item(item) {}

	WgItem*		m_item;
};


/// WgItemPixmapRes //////////////////////////////////////////////////////
class WgItemPixmapRes : public WgItemRes
{
public:
	WgItemPixmapRes(WgResourceXML* parent, class WgItemPixmap* item = 0);

	class WgItemPixmap*	GetItem() { return (WgItemPixmap*)WgItemRes::GetItem(); }

	virtual void	Serialize(WgResourceSerializerXML& s);
	virtual void	Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s);
	static const char* TagName() { return "itempixmap"; }
};

/// WgItemRowRes /////////////////////////////////////////////////////////
class WgItemRowRes : public WgItemRes, WgItemHolderRes
{
public:
	WgItemRowRes(WgResourceXML* parent, class WgItemRow* item = 0);

	class WgItemRow*	GetItem() { return (WgItemRow*)WgItemRes::GetItem(); }

	virtual void	Accept(WgResourceVisitor* visitor)	{ WgItemRes::Accept(visitor); WgItemHolderRes::Accept(visitor); }
	virtual void	Serialize(WgResourceSerializerXML& s);
	virtual void	Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s);
	static const char* TagName() { return "itemrow"; }
};

/// WgItemStackRes ///////////////////////////////////////////////////////
class WgItemStackRes : public WgItemRes, WgItemHolderRes
{
public:
	WgItemStackRes(WgResourceXML* parent, class WgItemStack* item = 0);

	class WgItemStack*	GetItem() { return (WgItemStack*)WgItemRes::GetItem(); }

	virtual void	Accept(WgResourceVisitor* visitor)	{ WgItemRes::Accept(visitor); WgItemHolderRes::Accept(visitor); }
	virtual void	Serialize(WgResourceSerializerXML& s);
	virtual void	Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s);
	static const char* TagName() { return "itemstack"; }
};

/// WgItemTextRes ////////////////////////////////////////////////////////
class WgItemTextRes : public WgItemRes, public WgTextHolderRes
{
public:
	WgItemTextRes(WgResourceXML* parent, class WgItemText* item = 0);
	virtual ~WgItemTextRes();

	class WgItemText*	GetItem() { return (WgItemText*)WgItemRes::GetItem(); }

	virtual WgCharBuffer*	GetCharBuffer();

	virtual void	Accept(WgResourceVisitor* visitor)	{ WgItemRes::Accept(visitor); WgTextHolderRes::Accept(visitor); }
	virtual void	Serialize(WgResourceSerializerXML& s);
	virtual void	Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s);
	static const char* TagName() { return "itemtext"; }
private:
	WgCharBuffer* m_pCharBuffer;
};

/// WgItemWrapTextRes ////////////////////////////////////////////////////
class WgItemWrapTextRes : public WgItemRes, public WgTextHolderRes
{
public:
	WgItemWrapTextRes(WgResourceXML* parent, class WgItemWrapText* item = 0);
	virtual ~WgItemWrapTextRes();

	class WgItemWrapText*	GetItem() { return (WgItemWrapText*)WgItemRes::GetItem(); }

	virtual WgCharBuffer*	GetCharBuffer();

	virtual void	Accept(WgResourceVisitor* visitor)	{ WgItemRes::Accept(visitor); WgTextHolderRes::Accept(visitor); }
	virtual void	Serialize(WgResourceSerializerXML& s);
	virtual void	Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s);
	static const char* TagName() { return "itemwraptext"; }
private:
	WgCharBuffer* m_pCharBuffer;
};

//////////////////////////////////////////////////////////////////////////
/// WgMenuItemRes ////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class WgMenuItemRes : public WgResourceXML
{
public:

	class WgMenuItem*	GetItem() { return m_item; }
	void				SetItem(WgMenuItem*item) { m_item = item; }

	virtual void	Serialize(WgResourceSerializerXML& s);
	virtual void	Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s);

	virtual void	Accept(WgResourceVisitor* visitor)	{ visitor->Visit(this); }

protected:
	WgMenuItemRes(WgResourceXML* parent, WgMenuItem* item = 0) : WgResourceXML(parent), m_item(item) {}

	WgMenuItem*		m_item;
};

/// WgMenuSeparatorRes ///////////////////////////////////////////////////
class WgMenuSeparatorRes : public WgMenuItemRes
{
public:
	WgMenuSeparatorRes(WgResourceXML* parent, class WgMenuSeparator* item = 0);

	class WgMenuSeparator*	GetItem() { return (WgMenuSeparator*)WgMenuItemRes::GetItem(); }

	virtual void	Serialize(WgResourceSerializerXML& s);
	virtual void	Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s);
	static const char* TagName() { return "menuseparator"; }
};

/// WgMenuEntryRes ///////////////////////////////////////////////////////
class WgMenuEntryRes : public WgMenuItemRes
{
public:
	WgMenuEntryRes(WgResourceXML* parent, class WgMenuEntry* item = 0);

	class WgMenuEntry*	GetItem() { return (WgMenuEntry*)WgMenuItemRes::GetItem(); }

	void			Serialize(WgResourceSerializerXML& s, bool bOpenTag);

	virtual void	Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s);
	static const char* TagName() { return "menuentry"; }
private:
	virtual void	Serialize(WgResourceSerializerXML& s);
};

/// WgMenuCheckBoxRes ////////////////////////////////////////////////////
class WgMenuCheckBoxRes : public WgMenuEntryRes
{
public:
	WgMenuCheckBoxRes(WgResourceXML* parent, class WgMenuCheckBox* item = 0);
	class WgMenuCheckBox*	GetItem() { return (WgMenuCheckBox*)WgMenuItemRes::GetItem(); }

	virtual void	Serialize(WgResourceSerializerXML& s);
	virtual void	Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s);
	static const char* TagName() { return "menucheckbox"; }
};

/// WgMenuRadioButtonRes /////////////////////////////////////////////////
class WgMenuRadioButtonRes : public WgMenuEntryRes
{
public:
	WgMenuRadioButtonRes(WgResourceXML* parent, class WgMenuRadioButton* item = 0);

	class WgMenuRadioButton*	GetItem() { return (WgMenuRadioButton*)WgMenuItemRes::GetItem(); }

	virtual void	Serialize(WgResourceSerializerXML& s);
	virtual void	Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s);
	static const char* TagName() { return "menuradiobutton"; }
};

/// WgMenuSubMenuRes /////////////////////////////////////////////////////
class WgMenuSubMenuRes : public WgMenuEntryRes
{
public:
	WgMenuSubMenuRes(WgResourceXML* parent, class WgMenuSubMenu*item = 0);

	class WgMenuSubMenu*	GetItem() { return (WgMenuSubMenu*)WgMenuItemRes::GetItem(); }

	virtual void	Accept(WgResourceVisitor* visitor)	{ visitor->Visit(this); }

	virtual void	Serialize(WgResourceSerializerXML& s);
	virtual void	Deserialize(const WgXmlNode& xmlNode, WgResourceSerializerXML& s);
	static const char* TagName() { return "menusubmenu"; }
};


#endif
