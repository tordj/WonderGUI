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

#ifndef WG_RESDB_DOT_H
#define WG_RESDB_DOT_H

#include <string>
#include <vector>
#include <map>


#ifndef WG_OBJECT_DOT_H
#	include <wg_object.h>
#endif

#ifndef WG_WIDGET_DOT_H
#	include <wg_widget.h>
#endif

#ifndef WG_CHAIN_DOT_H
#	include <wg_chain.h>
#endif

#ifndef WG_TEXTPROP_DOT_H
#	include <wg_textprop.h>
#endif

#ifndef WG_SKIN_DOT_H
#	include <wg_skin.h>
#endif

#ifndef WG_GFXANIM_DOT_H
#	include <wg_gfxanim.h>
#endif

#ifndef WG_CARET_DOT_H
#	include <wg_caret.h>
#endif

#ifndef WG_GLYPHSET_DOT_H
#	include	<wg_glyphset.h>
#endif

class WgFont;
class WgResLoader;


class WgResDB;
typedef	WgStrongPtr<WgResDB,WgObjectPtr>	WgResDBPtr;
typedef	WgWeakPtr<WgResDB,WgObjectWeakPtr>	WgResDBWeakPtr;

class WgResDB : public WgObject
{
public:
	static WgResDBPtr	Create() { return WgResDBPtr(new WgResDB()); }

	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgResDBPtr	Cast( const WgObjectPtr& pObject );


	class MetaData
	{
	public:
		virtual ~MetaData() {}
		virtual MetaData* Clone() const = 0;

		virtual std::string GetAttribute(const std::string& name) const = 0;
		virtual bool		SetAttribute(const std::string& name, const std::string& value) = 0;
		virtual bool		HasAttribute(const std::string& name) const = 0;
	};

	struct Resource : public WgLink
	{
		Resource( const std::string& id, MetaData* meta) : id(id), meta(meta) {}
		virtual ~Resource() { delete meta; }

		std::string		id;
		MetaData*		meta;
	};

	template<typename T>
	struct ResWrapper : public Resource
	{
		LINK_METHODS( ResWrapper<T> );
		ResWrapper(const std::string& id, T res, MetaData* meta) : Resource(id,meta), res(res) {}
		T res;
	};

	template<typename T>
	struct ResWrapper2 : public Resource
	{
		LINK_METHODS( ResWrapper2<T> );
		ResWrapper2(const std::string& id, T res, const std::string& file, MetaData* meta) : Resource(id,meta), res(res), file(file) {}
		T res;
		std::string	file;
	};

	struct LegoSource : public Resource
	{
		LegoSource(const std::string& id, const std::string& surface, WgRect rect, Uint32 nStates, MetaData* meta) : Resource(id, meta), surface(surface), rect(rect), nStates(nStates) { }
		std::string surface;
		WgRect rect;
		Uint32 nStates;
	};

	struct ConnectRes : public Resource
	{
		LINK_METHODS( ConnectRes );
		ConnectRes( MetaData* meta) : Resource("", meta) {}
	};


	typedef ResWrapper2<WgResDBPtr>		ResDBRes;
	typedef ResWrapper2<WgSurfacePtr>	SurfaceRes;
	typedef ResWrapper2<WgGlyphsetPtr>	GlyphsetRes;
	typedef ResWrapper<WgFontPtr>		FontRes;
	typedef ResWrapper<WgGfxAnimPtr>	GfxAnimRes;
	typedef ResWrapper<WgCaretPtr>		CursorRes;
	typedef ResWrapper<WgTextpropPtr>	TextpropRes;
	typedef ResWrapper<WgColor>			ColorRes;
	typedef ResWrapper<WgSkinPtr>		SkinRes;
	typedef ResWrapper<WgWidgetPtr>		WidgetRes;
	typedef ResWrapper<void*>			DataSetRes;


	//----

	static std::string	GenerateName( const WgSurfacePtr& data );
	static std::string	GenerateName( const WgGlyphsetPtr& data );
	static std::string	GenerateName( const WgFontPtr& data );
	static std::string	GenerateName( const WgGfxAnimPtr& data );
	static std::string	GenerateName( const WgCaretPtr& data );
	static std::string	GenerateName( const WgColor data );
	static std::string	GenerateName( const WgTextpropPtr& data );
	static std::string	GenerateName( const WgSkinPtr& data );
	static std::string	GenerateName( const WgWidgetPtr& data );

	void				SetResLoader( WgResLoader * pLoader );
	void				Clear();

	void				ClearSurfaces();
	void				ClearWidgets();

	std::string			LoadString( const std::string& token );

	bool				AddResDb( const std::string& file, MetaData * pMetaData = 0 );
	bool				AddResDb( const WgResDBPtr& db, const std::string& file, MetaData * pMetaData = 0 );

	bool				AddSurface( const std::string& id, const std::string& file, MetaData * pMetaData, bool bRequired );
	bool				AddSurface( const std::string& id, const WgSurfacePtr& pSurf, const std::string& filename, MetaData * pMetaData = 0 );

	bool				AddGlyphset( const std::string& id, const std::string& file, MetaData * pMetaData = 0 );
	bool				AddGlyphset( const std::string& id, const WgGlyphsetPtr& pGlyphset, const std::string& file, MetaData * pMetaData = 0 );

	bool				AddFont( const std::string& id, const WgFontPtr& pFont, MetaData * pMetaData = 0 );
	bool				AddGfxAnim( const std::string& id, const WgGfxAnimPtr& pAnim, MetaData * pMetaData = 0 );
	bool				AddCursor( const std::string& id, const WgCaretPtr& pCursor, MetaData * pMetaData = 0 );
	bool				AddTextprop( const std::string& id, const WgTextpropPtr& pProp, MetaData * pMetaData = 0 );
	bool				AddColor( const std::string& id, WgColor col, MetaData * pMetaData = 0 );
	bool				AddLegoSource( const std::string& id, const std::string& surface, WgRect rect, Uint32 nStates, MetaData * pMetaData = 0 );
	bool				AddSkin( const std::string& id, const WgSkinPtr& pSkin, MetaData * pMetaData = 0 );
	bool				AddConnect( MetaData * pMetaData );
	bool				AddWidget( const std::string& id, const WgWidgetPtr& pWidget, MetaData * pMetaData = 0 );
	bool				AddDataSet( const std::string& id, MetaData * pMetaData );

	bool				RemoveSurface( const std::string& id );
	bool				RemoveGlyphset( const std::string& id );
	bool				RemoveFont( const std::string& id );
	bool				RemoveGfxAnim( const std::string& id );
	bool				RemoveCursor( const std::string& id );
	bool				RemoveTextprop( const std::string& id );
	bool				RemoveColor( const std::string& id );
	bool				RemoveLegoSource( const std::string& id );
	bool				RemoveSkin( const std::string& id );
	bool				RemoveMenuitem( const std::string& id );
	bool				RemoveTab( const std::string& id );
	bool				RemoveConnect( const std::string& id );
	bool				RemoveResDB( const std::string& id );
	bool				RemoveWidget( const std::string& id );
	bool				RemoveDataSet( const std::string& id );

	bool				RemoveSurface( SurfaceRes * pRes );
	bool				RemoveGlyphset( GlyphsetRes * pRes );
	bool				RemoveFont( FontRes * pRes );
	bool				RemoveGfxAnim( GfxAnimRes * pRes );
	bool				RemoveCursor( CursorRes * pRes );
	bool				RemoveTextprop( TextpropRes * pRes );
	bool				RemoveColor( ColorRes * pRes );
	bool				RemoveLegoSource( LegoSource * pRes );
	bool				RemoveSkin( SkinRes * pRes );
	bool				RemoveConnect( ConnectRes* pRes );
	bool				RemoveResDB( ResDBRes* pRes );
	bool				RemoveWidget( WidgetRes * pRes );
	bool				RemoveDataSet( DataSetRes * pRes );



	WgSurfacePtr		GetSurface( const std::string& id ) const;
	WgGlyphsetPtr		GetGlyphset( const std::string& id ) const;
	WgFontPtr			GetFont( const std::string& id ) const;
	WgGfxAnimPtr		GetGfxAnim( const std::string& id ) const;
	WgCaretPtr			GetCursor( const std::string& id ) const;
	WgTextpropPtr		GetTextprop( const std::string& id ) const;
	WgColor				GetColor( const std::string& id ) const;
	WgSkinPtr			GetSkin( const std::string& id ) const;
	WgWidgetPtr			GetWidget( const std::string& id ) const;
	MetaData*			GetDataSet( const std::string& id ) const;
	WgWidgetPtr			CloneWidget( const std::string& id ) const;


	ResDBRes *			GetResDbRes( const std::string& id ) const;
	SurfaceRes *		GetResSurface( const std::string& id ) const;
	GlyphsetRes *		GetResGlyphset( const std::string& id ) const;
	FontRes *			GetResFont( const std::string& id ) const;
	GfxAnimRes *		GetResGfxAnim( const std::string& id ) const;
	CursorRes *			GetResCursor( const std::string& id ) const;
	TextpropRes *		GetResTextprop( const std::string& id ) const;
	ColorRes *			GetResColor( const std::string& id ) const;
	LegoSource *		GetLegoSource( const std::string& id ) const;
	SkinRes *			GetResSkin( const std::string& id ) const;
	ConnectRes *		GetResConnect( const std::string& id ) const;
	WidgetRes *			GetResWidget( const std::string& id ) const;
	DataSetRes *		GetResDataSet( const std::string& id ) const;

	ResDBRes *			FindResDbRes( const WgResDBPtr& data ) const;
	SurfaceRes *		FindResSurface( const WgSurfacePtr& data ) const;
	GlyphsetRes *		FindResGlyphset( const WgGlyphsetPtr& data ) const;
	FontRes *			FindResFont( const WgFontPtr& data ) const;
	GfxAnimRes *		FindResGfxAnim( const WgGfxAnimPtr& data ) const;
	CursorRes *			FindResCursor( const WgCaretPtr& data ) const;
	TextpropRes *		FindResTextprop( const WgTextpropPtr& data ) const;
	ColorRes *			FindResColor( const WgColor col ) const;
	SkinRes *			FindResSkin( const WgSkinPtr& data ) const;
	WidgetRes *			FindResWidget( const WgWidgetPtr& data ) const;

	std::string			FindSurfaceId( const WgSurfacePtr& data ) const		{ SurfaceRes * r =	FindResSurface(data); return r ? r->id : ""; }
	std::string			FindGlyphsetId( const WgGlyphsetPtr& data ) const		{ GlyphsetRes * r =	FindResGlyphset(data); return r ? r->id : ""; }
	std::string			FindFontId( const WgFontPtr& data ) const				{ FontRes *	r =		FindResFont(data); return r ? r->id : ""; }
	std::string			FindGfxAnimId( const WgGfxAnimPtr& data ) const		{ GfxAnimRes *	r =		FindResGfxAnim(data); return r ? r->id : ""; }
	std::string			FindCursorId( const WgCaretPtr& data ) const			{ CursorRes *	r =	FindResCursor(data); return r ? r->id : ""; }
	std::string			FindTextpropId( const WgTextpropPtr& data ) const	{ TextpropRes *r =  FindResTextprop(data); return r ? r->id : ""; }
	std::string			FindColorId( const WgColor data ) const				{ ColorRes *r =		FindResColor(data); return r ? r->id : ""; }
	std::string			FindSkinId( const WgSkinPtr& data ) const			{ SkinRes *r =		FindResSkin(data); return r ? r->id : ""; }
	std::string			FindWidgetId( const WgWidgetPtr& data ) const			{ WidgetRes *r =	FindResWidget(data); return r ? r->id : ""; }

	inline ResDBRes *		GetFirstResDBRes() const { return m_resDbs.First(); }
	inline SurfaceRes *		GetFirstResSurface() const { return m_surfaces.First(); }
	inline GlyphsetRes *	GetFirstResGlyphset() const { return m_glyphsets.First(); }
	inline FontRes *		GetFirstResFont() const { return m_fonts.First(); }
	inline GfxAnimRes *		GetFirstResGfxAnim() const { return m_gfxAnims.First(); }
	inline CursorRes *		GetFirstResCursor() const { return m_cursors.First(); }
	inline TextpropRes *	GetFirstResTextprop() const { return m_textProps.First(); }
	inline ColorRes *		GetFirstResColor() const { return m_colors.First(); }
	inline SkinRes *		GetFirstResSkin() const { return m_skins.First(); }
	inline ConnectRes *		GetFirstResConnect() const { return m_connects.First(); }
	inline WidgetRes *		GetFirstResWidget() const { return m_widgets.First(); }
	inline DataSetRes *		GetFirstResDataSet() const { return m_dataSets.First(); }


private:
	WgResDB();
	virtual ~WgResDB();

	typedef std::map<std::string, ResDBRes*>		ResDBMap;
	typedef std::map<std::string, SurfaceRes*>		SurfMap;
	typedef std::map<std::string, GlyphsetRes*>		GlyphMap;
	typedef std::map<std::string, FontRes*>			FontMap;
	typedef std::map<std::string, GfxAnimRes*>		GfxAnimMap;
	typedef std::map<std::string, CursorRes*>		CursorMap;
	typedef std::map<std::string, TextpropRes*>		PropMap;
	typedef std::map<std::string, ColorRes*>		ColMap;
	typedef std::map<std::string, LegoSource*>		LegoMap;
	typedef std::map<std::string, SkinRes*>			SkinMap;
	typedef std::map<std::string, ConnectRes*>		ConnectMap;
	typedef std::map<std::string, WidgetRes*>		WidgetMap;
	typedef std::map<std::string, DataSetRes*>		DataSetMap;

	WgChain<ResDBRes>		m_resDbs;
	WgChain<SurfaceRes>		m_surfaces;
	WgChain<GlyphsetRes>	m_glyphsets;
	WgChain<FontRes>		m_fonts;
	WgChain<GfxAnimRes>		m_gfxAnims;
	WgChain<CursorRes>		m_cursors;
	WgChain<TextpropRes>	m_textProps;
	WgChain<ColorRes>		m_colors;
	WgChain<LegoSource>		m_legos;
	WgChain<SkinRes>		m_skins;
	WgChain<ConnectRes>		m_connects;
	WgChain<WidgetRes>		m_widgets;
	WgChain<DataSetRes>		m_dataSets;

	ResDBMap		m_mapResDBs;
	ColMap			m_mapColors;
	SurfMap			m_mapSurfaces;
	GlyphMap		m_mapGlyphsets;
	FontMap			m_mapFonts;
	GfxAnimMap		m_mapGfxAnims;
	CursorMap		m_mapCursors;
	PropMap			m_mapTextprops;
	LegoMap			m_mapLegoSources;
	SkinMap			m_mapSkins;
	ConnectMap		m_mapConnects;
	WidgetMap		m_mapWidgets;
	DataSetMap		m_mapDataSets;

	WgResLoader *	m_pResLoader;
};



#endif //WG_RESDB_DOT_H
