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


#ifdef WG_LEGACY
#	ifndef WG_WIDGET_DOT_H
#	   include <wg_widget.h>
#	endif
#else
#	ifndef WG_GIZMO_DOT_H
#		include <wg_gizmo.h>
#	endif
#endif

#ifndef WG_CHAIN_DOT_H
#	include <wg_chain.h>
#endif

#ifndef WG_TEXTPROP_DOT_H
#	include <wg_textprop.h>
#endif

#ifndef WG_BLOCKSET_DOT_H
#	include <wg_blockset.h>
#endif

#ifndef WG_COLORSET_DOT_H
#	include <wg_colorset.h>
#endif


class WgSurface;
class WgGlyphSet;
class WgFont;
class WgAnim;
class WgCursor;
class WgResLoader;
class WgTab;
class WgTextManager;
class WgSkinManager;

class WgResDB
{
public:
	WgResDB();
	~WgResDB();


	class MetaData
	{
	public:
		virtual ~MetaData() {}
		virtual MetaData* Clone() const = 0;

		virtual std::string GetAttribute(const std::string& name) const = 0;
		virtual bool		SetAttribute(const std::string& name, const std::string& value) = 0;
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


	typedef ResWrapper2<WgResDB*>		ResDBRes;
	typedef ResWrapper2<WgSurface*>		SurfaceRes;
	typedef ResWrapper2<WgGlyphSet*>	GlyphSetRes;
	typedef ResWrapper<WgFont*>			FontRes;
	typedef ResWrapper<WgAnim*>			AnimRes;
	typedef ResWrapper<WgCursor*>		CursorRes;
	typedef ResWrapper<WgTextPropPtr>	TextPropRes;
	typedef ResWrapper<WgColor>			ColorRes;
	typedef ResWrapper<WgColorSetPtr>	ColorSetRes;
	typedef ResWrapper<WgBlockSetPtr>	BlockSetRes;
	typedef ResWrapper<WgMenuItem*>		MenuItemRes;
	typedef ResWrapper<WgTab*>			TabRes;
	typedef ResWrapper<WgTextManager*>	TextManagerRes;
	typedef ResWrapper<WgSkinManager*>	SkinManagerRes;
	typedef ResWrapper<WgGizmo*>		GizmoRes;

	//----

	static std::string	GenerateName( const WgSurface * data );
	static std::string	GenerateName( const WgGlyphSet * data );
	static std::string	GenerateName( const WgFont * data );
	static std::string	GenerateName( const WgAnim * data );
	static std::string	GenerateName( const WgCursor * data );
	static std::string	GenerateName( const WgColor data );
	static std::string	GenerateName( const WgColorSetPtr& data );
	static std::string	GenerateName( const WgTextPropPtr& data );
	static std::string	GenerateName( const WgBlockSetPtr& data );
	static std::string	GenerateName( const WgMenuItem* data );
	static std::string	GenerateName( const WgTextManager* data );
	static std::string	GenerateName( const WgSkinManager* data );
	static std::string	GenerateName( const WgGizmo * data );

	void				SetResLoader( WgResLoader * pLoader );
	void				Clear();

	void				DestroyFonts();
	void				DestroyAnims();
	void				DestroyCursors();
	void				DestroyTextManagers();

	void				ClearGizmos();// debug function

#ifdef WG_LEGACY
	void				ClearWidgets();// debug function
	void				ClearConnects();// debug function

	bool				Connect( const std::string& id, WgWidget* emitter, const std::string& action, WgWidget* receiver);
#endif

	std::string			LoadString( const std::string& token );

	bool				AddResDb( const std::string& file, MetaData * pMetaData = 0 );
	bool				AddResDb( WgResDB* db, const std::string& file, MetaData * pMetaData = 0 );

	bool				AddSurface( const std::string& id, const std::string& file, MetaData * pMetaData, bool bRequired );
	bool				AddSurface( const std::string& id, WgSurface * pSurf, const std::string& filename, MetaData * pMetaData = 0 );

	bool				AddGlyphSet( const std::string& id, const std::string& file, MetaData * pMetaData = 0 );
	bool				AddGlyphSet( const std::string& id, WgGlyphSet * pGlyphSet, const std::string& file, MetaData * pMetaData = 0 );

	bool				AddFont( const std::string& id, WgFont * pFont, MetaData * pMetaData = 0 );
	bool				AddAnim( const std::string& id, WgAnim * pAnim, MetaData * pMetaData = 0 );
	bool				AddCursor( const std::string& id, WgCursor * pCursor, MetaData * pMetaData = 0 );
	bool				AddTextProp( const std::string& id, const WgTextPropPtr& pProp, MetaData * pMetaData = 0 );
	bool				AddColor( const std::string& id, WgColor col, MetaData * pMetaData = 0 );
	bool				AddColorSet( const std::string& id, const WgColorSetPtr& pColorSet, MetaData * pMetaData = 0 );
	bool				AddLegoSource( const std::string& id, const std::string& surface, WgRect rect, Uint32 nStates, MetaData * pMetaData = 0 );
	bool				AddBlockSet( const std::string& id, const WgBlockSetPtr& pBlockSet, MetaData * pMetaData = 0 );
	bool				AddMenuItem( const std::string& id, WgMenuItem * pMenuItem, MetaData * pMetaData = 0 );
	bool				AddTab( const std::string& id, WgTab * pTab, MetaData * pMetaData = 0 );
	bool				AddTextManager( const std::string& id, WgTextManager* pTextManager, MetaData * pMetaData = 0 );
	bool				AddSkinManager( const std::string& id, WgSkinManager* pSkinManager, MetaData * pMetaData = 0 );
	bool				AddConnect( MetaData * pMetaData );
	bool				AddGizmo( const std::string& id, WgGizmo * pGizmo, MetaData * pMetaData = 0 );

	bool				RemoveSurface( const std::string& id );
	bool				RemoveGlyphSet( const std::string& id );
	bool				RemoveFont( const std::string& id );
	bool				RemoveAnim( const std::string& id );
	bool				RemoveCursor( const std::string& id );
	bool				RemoveTextProp( const std::string& id );
	bool				RemoveColor( const std::string& id );
	bool				RemoveColorSet( const std::string& id );
	bool				RemoveLegoSource( const std::string& id );
	bool				RemoveBlockSet( const std::string& id );
	bool				RemoveMenuItem( const std::string& id );
	bool				RemoveTab( const std::string& id );
	bool				RemoveTextManager( const std::string& id );
	bool				RemoveSkinManager( const std::string& id );
	bool				RemoveConnect( const std::string& id );
	bool				RemoveResDB( const std::string& id );
	bool				RemoveGizmo( const std::string& id );

	bool				RemoveSurface( SurfaceRes * pRes );
	bool				RemoveGlyphSet( GlyphSetRes * pRes );
	bool				RemoveFont( FontRes * pRes );
	bool				RemoveAnim( AnimRes * pRes );
	bool				RemoveCursor( CursorRes * pRes );
	bool				RemoveTextProp( TextPropRes * pRes );
	bool				RemoveColor( ColorRes * pRes );
	bool				RemoveColorSet( ColorSetRes * pRes );
	bool				RemoveLegoSource( LegoSource * pRes );
	bool				RemoveBlockSet( BlockSetRes * pRes );
	bool				RemoveMenuItem( MenuItemRes * pRes );
	bool				RemoveTab( TabRes * pRes );
	bool				RemoveTextManager( TextManagerRes * pRes );
	bool				RemoveSkinManager( SkinManagerRes * pRes );
	bool				RemoveConnect( ConnectRes* pRes );
	bool				RemoveResDB( ResDBRes* pRes );
	bool				RemoveGizmo( GizmoRes * pRes );



	WgSurface *			GetSurface( const std::string& id ) const;
	WgGlyphSet *		GetGlyphSet( const std::string& id ) const;
	WgFont *			GetFont( const std::string& id ) const;
	WgAnim *			GetAnim( const std::string& id ) const;
	WgCursor *			GetCursor( const std::string& id ) const;
	WgTextPropPtr		GetTextProp( const std::string& id ) const;
	WgColor				GetColor( const std::string& id ) const;
	WgColorSetPtr		GetColorSet( const std::string& id ) const;
	WgBlockSetPtr		GetBlockSet( const std::string& id ) const;
	WgTextManager *		GetTextManager( const std::string& id ) const;
	WgSkinManager *		GetSkinManager( const std::string& id ) const;
	WgMenuItem *		GetMenuItem( const std::string& id ) const;
	WgTab*				GetTab( const std::string& id ) const;
	WgGizmo*			GetGizmo( const std::string& id ) const;


#ifdef WG_LEGACY
	typedef ResWrapper<WgWidget*>		WidgetRes;
	typedef ResWrapper<WgItem*>			ItemRes;

	static std::string	GenerateName( const WgWidget* data );
	static std::string	GenerateName( const WgItem* data );

	bool				AddWidget( const std::string& id, WgWidget * pWidget, MetaData * pMetaData = 0 );
	bool				AddItem( const std::string& id, WgItem * pItem, MetaData * pMetaData = 0 );

	bool				RemoveWidget( const std::string& id );
	bool				RemoveItem( const std::string& id );

	bool				RemoveWidget( WidgetRes * pRes );
	bool				RemoveItem( ItemRes * pRes );

	WgWidget *			GetWidget( const std::string& id ) const;
	WgItem *			GetItem( const std::string& id ) const;

	WidgetRes *			GetResWidget( const std::string& id ) const;
	ItemRes *			GetResItem( const std::string& id ) const;

	WidgetRes *			FindResWidget( const WgWidget* data ) const;
	ItemRes *			FindResItem( const WgItem* data ) const;

	std::string			FindWidgetId( const WgWidget* data ) const			{ WidgetRes *r =	FindResWidget(data); return r ? r->id : ""; }
	std::string			FindItemId( const WgItem* data ) const				{ ItemRes *	r =		FindResItem(data); return r ? r->id : ""; }

	inline WidgetRes *		GetFirstResWidget() const { return m_widgets.First(); }
	inline ItemRes *		GetFirstResItem() const { return m_items.First(); }

	template<typename T> T* GetCastWidget(const std::string& id) const
	{
		WgWidget * widget = GetWidget(id);
		if(widget)
		{
			if(T::GetMyType() == widget->Type())
				return static_cast<T*>(widget);
		}
		return 0;
	}
#endif


	ResDBRes *			GetResDbRes( const std::string& id ) const;
	SurfaceRes *		GetResSurface( const std::string& id ) const;
	GlyphSetRes *		GetResGlyphSet( const std::string& id ) const;
	FontRes *			GetResFont( const std::string& id ) const;
	AnimRes *			GetResAnim( const std::string& id ) const;
	CursorRes *			GetResCursor( const std::string& id ) const;
	TextPropRes *		GetResTextProp( const std::string& id ) const;
	ColorRes *			GetResColor( const std::string& id ) const;
	ColorSetRes *		GetResColorSet( const std::string& id ) const;
	LegoSource *		GetLegoSource( const std::string& id ) const;
	BlockSetRes *		GetResBlockSet( const std::string& id ) const;
	MenuItemRes *		GetResMenuItem( const std::string& id ) const;
	TabRes *			GetResTab( const std::string& id ) const;
	TextManagerRes *	GetResTextManager( const std::string& id ) const;
	SkinManagerRes *	GetResSkinManager( const std::string& id ) const;
	ConnectRes *		GetResConnect( const std::string& id ) const;
	GizmoRes *			GetResGizmo( const std::string& id ) const;

	ResDBRes *			FindResDbRes( const WgResDB * data ) const;
	SurfaceRes *		FindResSurface( const WgSurface * data ) const;
	GlyphSetRes *		FindResGlyphSet( const WgGlyphSet * data ) const;
	FontRes *			FindResFont( const WgFont * data ) const;
	AnimRes *			FindResAnim( const WgAnim * data ) const;
	CursorRes *			FindResCursor( const WgCursor * data ) const;
	TextPropRes *		FindResTextProp( const WgTextPropPtr& data ) const;
	ColorRes *			FindResColor( const WgColor col ) const;
	ColorSetRes *		FindResColorSet( const WgColorSetPtr& data ) const;
	BlockSetRes *		FindResBlockSet( const WgBlockSetPtr& data ) const;
	MenuItemRes *		FindResMenuItem( const WgMenuItem* data ) const;
	TabRes *			FindResTab( const WgTab* data ) const;
	TextManagerRes *	FindResTextManager( const WgTextManager* data ) const;
	SkinManagerRes *	FindResSkinManager( const WgSkinManager* data ) const;
	GizmoRes *			FindResGizmo( const WgGizmo* data ) const;

	std::string			FindSurfaceId( const WgSurface * data ) const		{ SurfaceRes * r =	FindResSurface(data); return r ? r->id : ""; }
	std::string			FindGlyphSetId( const WgGlyphSet * data ) const		{ GlyphSetRes * r =	FindResGlyphSet(data); return r ? r->id : ""; }
	std::string			FindFontId( const WgFont * data ) const				{ FontRes *	r =		FindResFont(data); return r ? r->id : ""; }
	std::string			FindAnimId( const WgAnim * data ) const				{ AnimRes *	r =		FindResAnim(data); return r ? r->id : ""; }
	std::string			FindCursorId( const WgCursor * data ) const			{ CursorRes *	r =	FindResCursor(data); return r ? r->id : ""; }
	std::string			FindTextPropId( const WgTextPropPtr& data ) const	{ TextPropRes *r =  FindResTextProp(data); return r ? r->id : ""; }
	std::string			FindColorId( const WgColor data ) const				{ ColorRes *r =		FindResColor(data); return r ? r->id : ""; }
	std::string			FindColorSetId( const WgColorSetPtr& data ) const	{ ColorSetRes *r =  FindResColorSet(data); return r ? r->id : ""; }
	std::string			FindBlockSetId( const WgBlockSetPtr& data ) const	{ BlockSetRes *r =  FindResBlockSet(data); return r ? r->id : ""; }
	std::string			FindMenuItemId( const WgMenuItem* data ) const		{ MenuItemRes *r =  FindResMenuItem(data); return r ? r->id : ""; }
	std::string			FindTabId( const WgTab* data ) const				{ TabRes *r =		FindResTab(data); return r ? r->id : ""; }
	std::string			FindTextManagerId( const WgTextManager* data ) const{ TextManagerRes *r =  	FindResTextManager(data); return r ? r->id : ""; }
	std::string			FindSkinManagerId( const WgSkinManager* data ) const{ SkinManagerRes *r =  	FindResSkinManager(data); return r ? r->id : ""; }
	std::string			FindGizmoId( const WgGizmo* data ) const			{ GizmoRes *r =	FindResGizmo(data); return r ? r->id : ""; }

	inline ResDBRes *		GetFirstResDBRes() const { return m_resDbs.First(); }
	inline SurfaceRes *		GetFirstResSurface() const { return m_surfaces.First(); }
	inline GlyphSetRes *	GetFirstResGlyphSet() const { return m_glyphSets.First(); }
	inline FontRes *		GetFirstResFont() const { return m_fonts.First(); }
	inline AnimRes *		GetFirstResAnim() const { return m_anims.First(); }
	inline CursorRes *		GetFirstResCursor() const { return m_cursors.First(); }
	inline TextPropRes *	GetFirstResTextProp() const { return m_textProps.First(); }
	inline ColorRes *		GetFirstResColor() const { return m_colors.First(); }
	inline ColorSetRes *	GetFirstResColorSet() const { return m_colorSets.First(); }
	inline BlockSetRes *	GetFirstResBlockSet() const { return m_blockSets.First(); }
	inline MenuItemRes *	GetFirstResMenuItem() const { return m_menuItems.First(); }
	inline TabRes *			GetFirstResTab() const { return m_tabs.First(); }
	inline TextManagerRes *	GetFirstResTextManager() const { return m_textManagers.First(); }
	inline SkinManagerRes *	GetFirstResSkinManager() const { return m_skinManagers.First(); }
	inline ConnectRes *		GetFirstResConnect() const { return m_connects.First(); }
	inline GizmoRes *		GetFirstResGizmo() const { return m_gizmos.First(); }


	template<typename T> T* GetCastGizmo(const std::string& id) const
	{
		WgGizmo * pGizmo = GetGizmo(id);
		if(pGizmo)
		{
			if(T::GetMyType() == pGizmo->Type())
				return static_cast<T*>(pGizmo);
		}
		return 0;
	}


private:

#ifdef WG_LEGACY
	typedef std::map<std::string, WidgetRes*>		WdgMap;
	typedef std::map<std::string, ItemRes*>			ItemMap;

	WgChain<WidgetRes>		m_widgets;
	WgChain<ItemRes>		m_items;

	WdgMap			m_mapWidgets;
	ItemMap			m_mapItems;
#endif

	typedef std::map<std::string, ResDBRes*>		ResDBMap;
	typedef std::map<std::string, SurfaceRes*>		SurfMap;
	typedef std::map<std::string, GlyphSetRes*>		GlyphMap;
	typedef std::map<std::string, FontRes*>			FontMap;
	typedef std::map<std::string, AnimRes*>			AnimMap;
	typedef std::map<std::string, CursorRes*>		CursorMap;
	typedef std::map<std::string, TextPropRes*>		PropMap;
	typedef std::map<std::string, ColorRes*>		ColMap;
	typedef std::map<std::string, ColorSetRes*>		ColSetMap;
	typedef std::map<std::string, LegoSource*>		LegoMap;
	typedef std::map<std::string, BlockSetRes*>		BlockMap;
	typedef std::map<std::string, MenuItemRes*>		MenuItemMap;
	typedef std::map<std::string, TabRes*>			TabMap;
	typedef std::map<std::string, TextManagerRes*>	TextManagerMap;
	typedef std::map<std::string, SkinManagerRes*>	SkinManagerMap;
	typedef std::map<std::string, ConnectRes*>		ConnectMap;
	typedef std::map<std::string, GizmoRes*>		GizmoMap;

	WgChain<ResDBRes>		m_resDbs;
	WgChain<SurfaceRes>		m_surfaces;
	WgChain<GlyphSetRes>	m_glyphSets;
	WgChain<FontRes>		m_fonts;
	WgChain<AnimRes>		m_anims;
	WgChain<CursorRes>		m_cursors;
	WgChain<TextPropRes>	m_textProps;
	WgChain<ColorRes>		m_colors;
	WgChain<ColorSetRes>	m_colorSets;
	WgChain<LegoSource>		m_legos;
	WgChain<BlockSetRes>	m_blockSets;
	WgChain<MenuItemRes>	m_menuItems;
	WgChain<TabRes>			m_tabs;
	WgChain<TextManagerRes>	m_textManagers;
	WgChain<SkinManagerRes>	m_skinManagers;
	WgChain<ConnectRes>		m_connects;
	WgChain<GizmoRes>		m_gizmos;

	ResDBMap		m_mapResDBs;
	ColMap			m_mapColors;
	ColSetMap		m_mapColorSets;
	SurfMap			m_mapSurfaces;
	GlyphMap		m_mapGlyphSets;
	FontMap			m_mapFonts;
	AnimMap			m_mapAnims;
	CursorMap		m_mapCursors;
	PropMap			m_mapTextProps;
	LegoMap			m_mapLegoSources;
	BlockMap		m_mapBlockSets;
	MenuItemMap		m_mapMenuItems;
	TabMap			m_mapTabs;
	TextManagerMap	m_mapTextManagers;
	SkinManagerMap	m_mapSkinManagers;
	ConnectMap		m_mapConnects;
	GizmoMap		m_mapGizmos;

	WgResLoader *	m_pResLoader;
};



#endif //WG_RESDB_DOT_H
