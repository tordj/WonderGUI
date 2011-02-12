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

#include <stdlib.h>
#include <wg_resdb.h>
#include <wg_resources_xml.h>
#include <wg_resloader.h>
#include <assert.h>
#include <wg_font.h>
//____ () _________________________________________________________

WgResDB::WgResDB() :
	m_pResLoader(0)
{
}

//____ () _________________________________________________________

WgResDB::~WgResDB()
{
	DestroyTextManagers();
	Clear();
}

//____ () _________________________________________________________

void WgResDB::Clear()
{
	// Clear the quick-reference maps

	m_mapColors.clear();
	m_mapSurfaces.clear();
	m_mapGlyphSets.clear();
	m_mapFonts.clear();
	m_mapAnims.clear();
	m_mapCursors.clear();
	m_mapTextProps.clear();
	m_mapLegoSources.clear();
	m_mapBlockSets.clear();
#ifdef WG_LEGACY
	m_mapWidgets.clear();
	m_mapItems.clear();
#endif
	m_mapMenuItems.clear();
	m_mapTabs.clear();
	m_mapTextManagers.clear();
	m_mapSkinManagers.clear();
	m_mapConnects.clear();
	m_mapResDBs.clear();

	// Clear the linked lists, this will also delete the ResWrapper objects
	// along with their meta-data but NOT the resources themselves.
	// This will cause memory leaks until all resources are properly equipped
	// with smartpointers.

	m_surfaces.Clear();
	m_glyphSets.Clear();
	m_fonts.Clear();
	m_anims.Clear();
	m_cursors.Clear();
	m_textProps.Clear();
	m_colors.Clear();
	m_legos.Clear();
	m_blockSets.Clear();
#ifdef WG_LEGACY
	m_widgets.Clear();
	m_items.Clear();
#endif
	m_menuItems.Clear();
	m_tabs.Clear();
	m_textManagers.Clear();
	m_skinManagers.Clear();
	m_connects.Clear();
	m_resDbs.Clear();
}

void WgResDB::DestroyFonts()
{
	for(FontRes* res = GetFirstResFont(); res; res = res->Next())
		delete res->res;
}

void WgResDB::DestroyAnims()
{
	for(AnimRes* res = GetFirstResAnim(); res; res = res->Next())
		delete res->res;
}

void WgResDB::DestroyCursors()
{
	for(CursorRes* res = GetFirstResCursor(); res; res = res->Next())
		delete res->res;
}

void WgResDB::DestroyTextManagers()
{
	for(TextManagerRes* res = GetFirstResTextManager(); res; res = res->Next())
		delete res->res;
}

#ifdef WG_LEGACY
void WgResDB::ClearWidgets()
{
	m_mapWidgets.clear();
	m_widgets.Clear();
}

void WgResDB::ClearConnects()
{
	m_connects.Clear();
	m_mapConnects.clear();
}
#endif

//____ () _________________________________________________________

std::string	WgResDB::GenerateName( const WgSurface * data )
{
	static int nGenerated = 0;
	char pBuf[100];
	return std::string("_surf__") + WgTextTool::itoa(++nGenerated, pBuf, 10);
}

std::string	WgResDB::GenerateName( const WgGlyphSet * data )
{
	static int nGenerated = 0;
	char pBuf[100];
	return std::string("_glyphset__") + WgTextTool::itoa(++nGenerated, pBuf, 10);
}

std::string	WgResDB::GenerateName( const WgFont * data )
{
	static int nGenerated = 0;
	char pBuf[100];
	return std::string("_font__") + WgTextTool::itoa(++nGenerated, pBuf, 10);
}

std::string	WgResDB::GenerateName( const WgAnim * data )
{
	static int nGenerated = 0;
	char pBuf[100];
	return std::string("_anim__") + WgTextTool::itoa(++nGenerated, pBuf, 10);
}

std::string	WgResDB::GenerateName( const WgCursor * data )
{
	static int nGenerated = 0;
	char pBuf[100];
	return std::string("_cursor__") + WgTextTool::itoa(++nGenerated, pBuf, 10);
}

std::string	WgResDB::GenerateName( const WgTextPropPtr data )
{
	static int nGenerated = 0;
	char pBuf[100];
	return std::string("_textprop__") + WgTextTool::itoa(++nGenerated, pBuf, 10);
}

std::string	WgResDB::GenerateName( const WgBlockSetPtr data )
{
	static int nGenerated = 0;
	char pBuf[100];
	return std::string("_blockset__") + WgTextTool::itoa(++nGenerated, pBuf, 10);
}

#ifdef WG_LEGACY
std::string	WgResDB::GenerateName( const WgWidget* data )
{
	static int nGenerated = 0;
	char pBuf[100];
	return std::string("_widget__") + WgTextTool::itoa(++nGenerated, pBuf, 10);
}

std::string	WgResDB::GenerateName( const WgItem* data )
{
	static int nGenerated = 0;
	char pBuf[100];
	return std::string("_item__") + WgTextTool::itoa(++nGenerated, pBuf, 10);
}
#endif

std::string	WgResDB::GenerateName( const WgMenuItem* data )
{
	static int nGenerated = 0;
	char pBuf[100];
	return std::string("_menuitem__") + WgTextTool::itoa(++nGenerated, pBuf, 10);
}

std::string	WgResDB::GenerateName( const WgTextManager* data )
{
	static int nGenerated = 0;
	char pBuf[100];
	return std::string("_textmanager__") + WgTextTool::itoa(++nGenerated, pBuf, 10);
}

std::string	WgResDB::GenerateName( const WgSkinManager* data )
{
	static int nGenerated = 0;
	char pBuf[100];
	return std::string("_skinmanager__") + WgTextTool::itoa(++nGenerated, pBuf, 10);
}

void WgResDB::SetResLoader( WgResLoader * pLoader )
{
	m_pResLoader = pLoader;
}

//____ () _________________________________________________________
bool WgResDB::AddResDb( const std::string& file, MetaData * pMetaData )
{
//	assert(GetResDbRes(file) == 0);

	if(GetResDbRes(file) == 0 && m_pResLoader)
	{
		WgResDB * pDb = m_pResLoader->LoadDb( file, this );
		// store resource even if load failed. could be an optional include
		ResDBRes* p = new ResDBRes(file, pDb, file, pMetaData);
		m_resDbs.PushBack(p);
		m_mapResDBs[file] = p;
		return true;
	}
	return false;
}

bool WgResDB::AddResDb( WgResDB* db, const std::string& file, MetaData * pMetaData )
{
	assert(GetResDbRes(file) == 0);

	if(GetResDbRes(file) == 0)
	{
		ResDBRes* p = new ResDBRes(file, db, file, pMetaData);
		m_resDbs.PushBack(p);
		if(file.size())
			m_mapResDBs[file] = p;
		return true;
	}
	return false;
}

bool WgResDB::AddSurface( const std::string& id, const std::string& file, MetaData * pMetaData, bool bRequired )
{
	assert(m_mapSurfaces.find(id) == m_mapSurfaces.end());

	if(m_mapSurfaces.find(id) == m_mapSurfaces.end() && m_pResLoader)
	{
		WgSurface * pSurface = m_pResLoader->LoadSurface( file, bRequired );
		if( !pSurface )
			return false;

		SurfaceRes* p = new SurfaceRes(id, pSurface, file, pMetaData);
		m_surfaces.PushBack(p);
		if(id.size())
			m_mapSurfaces[id] = p;
		return true;
	}
	return false;
}

//____ () _________________________________________________________

bool WgResDB::AddSurface( const std::string& id, WgSurface * pSurface, const std::string& filename, MetaData * pMetaData )
{
	assert(m_mapSurfaces.find(id) == m_mapSurfaces.end());
	if(m_mapSurfaces.find(id) == m_mapSurfaces.end())
	{
		SurfaceRes* p = new SurfaceRes(id, pSurface, filename, pMetaData);
		m_surfaces.PushBack(p);
		if(id.size())
			m_mapSurfaces[id] = p;
		return true;
	}
	return false;
}


//____ () _________________________________________________________

bool WgResDB::AddGlyphSet( const std::string& id, const std::string& file, MetaData * pMetaData )
{
	assert(m_mapGlyphSets.find(id) == m_mapGlyphSets.end());

	if(m_mapGlyphSets.find(id) == m_mapGlyphSets.end() && m_pResLoader)
	{
		WgGlyphSet* pGlyphSet = m_pResLoader->LoadGlyphSet( file );
		if( !pGlyphSet )
			return false;

		GlyphSetRes* p = new GlyphSetRes(id, pGlyphSet, file, pMetaData);
		m_glyphSets.PushBack(p);
		if(id.size())
			m_mapGlyphSets[id] = p;
		return true;
	}
	return false;
}

bool WgResDB::AddGlyphSet( const std::string& id, WgGlyphSet * pGlyphSet, const std::string& file, MetaData * pMetaData )
{
	assert(m_mapGlyphSets.find(id) == m_mapGlyphSets.end());
	if(m_mapGlyphSets.find(id) == m_mapGlyphSets.end())
	{
		GlyphSetRes* p = new GlyphSetRes(id, pGlyphSet, file, pMetaData);
		m_glyphSets.PushBack(p);
		if(id.size())
			m_mapGlyphSets[id] = p;
		return true;
	}
	return false;
}

//____ () _________________________________________________________

bool WgResDB::AddFont( const std::string& id, WgFont * pFont, MetaData * pMetaData )
{
	assert(m_mapFonts.find(id) == m_mapFonts.end());
	if(m_mapFonts.find(id) == m_mapFonts.end())
	{
		FontRes* p = new FontRes(id, pFont, pMetaData);
		m_fonts.PushBack(p);
		if(id.size())
			m_mapFonts[id] = p;
		return true;
	}
	return false;
}

//____ () _________________________________________________________

bool WgResDB::AddAnim( const std::string& id, WgAnim * pAnim, MetaData * pMetaData )
{
	assert(m_mapAnims.find(id) == m_mapAnims.end());
	if(m_mapAnims.find(id) == m_mapAnims.end())
	{
		AnimRes* p = new AnimRes(id, pAnim, pMetaData);
		m_anims.PushBack(p);
		if(id.size())
			m_mapAnims[id] = p;
		return true;
	}
	return false;
}

//____ () _________________________________________________________

bool WgResDB::AddCursor( const std::string& id, WgCursor * pCursor, MetaData * pMetaData )
{
	assert(m_mapCursors.find(id) == m_mapCursors.end());
	if(m_mapCursors.find(id) == m_mapCursors.end())
	{
		CursorRes* p = new CursorRes(id, pCursor, pMetaData);
		m_cursors.PushBack(p);
		if(id.size())
			m_mapCursors[id] = p;
		return true;
	}
	return false;
}

//____ () _________________________________________________________

bool WgResDB::AddTextProp( const std::string& id, WgTextPropPtr pProp, MetaData * pMetaData )
{
	//assert(m_mapTextProps.find(id) == m_mapTextProps.end());
	if(m_mapTextProps.find(id) == m_mapTextProps.end())
	{
		TextPropRes* p = new TextPropRes(id, pProp, pMetaData);
		m_textProps.PushBack(p);
		if(id.size())
			m_mapTextProps[id] = p;
		return true;
	}
	return false;
}

//____ () _________________________________________________________

bool WgResDB::AddColor( const std::string& id, WgColor col, MetaData * pMetaData )
{
	assert(m_mapColors.find(id) == m_mapColors.end());
	if(m_mapColors.find(id) == m_mapColors.end())
	{
		ColorRes* p = new ColorRes(id, col, pMetaData);
		m_colors.PushBack(p);
		if(id.size())
			m_mapColors[id] = p;
		return true;
	}
	return false;
}

//____ () _________________________________________________________

bool WgResDB::AddLegoSource( const std::string& id, const std::string& surface, WgRect rect, Uint32 nStates, MetaData * pMetaData )
{
	assert(m_mapLegoSources.find(id) == m_mapLegoSources.end());
	if(m_mapLegoSources.find(id) == m_mapLegoSources.end())
	{
		LegoSource* p = new LegoSource(id, surface, rect, nStates, pMetaData);
		m_legos.PushBack(p);
		if(id.size())
			m_mapLegoSources[id] = p;
		return true;
	}
	return false;
}

#ifdef WG_LEGACY
bool WgResDB::Connect( const std::string& id, WgWidget* emitter, const std::string& action, WgWidget* receiver)
{
	assert(m_pResLoader);
	if(m_pResLoader)
	{
		m_pResLoader->LoadCallback(id, emitter, receiver);
		return true;
	}
	return false;
}
#endif

std::string WgResDB::LoadString( const std::string& token )
{
	return m_pResLoader->LoadString(token);
}

//____ () _________________________________________________________

bool WgResDB::AddBlockSet( const std::string& id, WgBlockSetPtr pBlockSet, MetaData * pMetaData )
{
	assert(m_mapBlockSets.find(id) == m_mapBlockSets.end());
	if(m_mapBlockSets.find(id) == m_mapBlockSets.end())
	{
		BlockSetRes* p = new BlockSetRes(id, pBlockSet, pMetaData);
		m_blockSets.PushBack(p);
		if(id.size())
			m_mapBlockSets[id] = p;
		return true;
	}
	return false;
}

#ifdef WG_LEGACY
//____ () _________________________________________________________

bool WgResDB::AddWidget( const std::string& id, WgWidget * pWidget, MetaData * pMetaData )
{
	assert(m_mapWidgets.find(id) == m_mapWidgets.end());
	if(m_mapWidgets.find(id) == m_mapWidgets.end())
	{
		WidgetRes* p = new WidgetRes(id, pWidget, pMetaData);
		m_widgets.PushBack(p);
		if(id.size())
			m_mapWidgets[id] = p;
		return true;
	}
	return false;
}

//____ () _________________________________________________________

bool WgResDB::AddItem( const std::string& id, WgItem * pItem, MetaData * pMetaData )
{
	assert(m_mapItems.find(id) == m_mapItems.end());
	if(m_mapItems.find(id) == m_mapItems.end())
	{
		ItemRes* p = new ItemRes(id, pItem, pMetaData);
		m_items.PushBack(p);
		if(id.size())
			m_mapItems[id] = p;
		return true;
	}
	return false;
}
#endif

//____ () _________________________________________________________

bool WgResDB::AddMenuItem( const std::string& id, WgMenuItem * pItem, MetaData * pMetaData )
{
	assert(m_mapMenuItems.find(id) == m_mapMenuItems.end());
	if(m_mapMenuItems.find(id) == m_mapMenuItems.end())
	{
		MenuItemRes* p = new MenuItemRes(id, pItem, pMetaData);
		m_menuItems.PushBack(p);
		if(id.size())
			m_mapMenuItems[id] = p;
		return true;
	}
	return false;
}

//____ () _________________________________________________________

bool WgResDB::AddTab( const std::string& id, WgTab * pItem, MetaData * pMetaData )
{
	assert(m_mapTabs.find(id) == m_mapTabs.end());
	if(m_mapTabs.find(id) == m_mapTabs.end())
	{
		TabRes* p = new TabRes(id, pItem, pMetaData);
		m_tabs.PushBack(p);
		if(id.size())
			m_mapTabs[id] = p;
		return true;
	}
	return false;
}

//____ () _________________________________________________________

bool WgResDB::AddTextManager( const std::string& id, WgTextManager* pManager, MetaData * pMetaData )
{
	assert(m_mapTextManagers.find(id) == m_mapTextManagers.end());
	if(m_mapTextManagers.find(id) == m_mapTextManagers.end())
	{
		TextManagerRes* p = new TextManagerRes(id, pManager, pMetaData);
		m_textManagers.PushBack(p);
		if(id.size())
			m_mapTextManagers[id] = p;
		return true;
	}
	return false;
}

//____ () _________________________________________________________

bool WgResDB::AddSkinManager( const std::string& id, WgSkinManager* pManager, MetaData * pMetaData )
{
	assert(m_mapSkinManagers.find(id) == m_mapSkinManagers.end());
	if(m_mapSkinManagers.find(id) == m_mapSkinManagers.end())
	{
		SkinManagerRes* p = new SkinManagerRes(id, pManager, pMetaData);
		m_skinManagers.PushBack(p);
		if(id.size())
			m_mapSkinManagers[id] = p;
		return true;
	}
	return false;
}


//____ () _________________________________________________________

bool WgResDB::AddConnect( MetaData * pMetaData )
{
	ConnectRes* p = new ConnectRes(pMetaData);
	m_connects.PushBack(p);
	return true;
}

//____ () _________________________________________________________

WgSurface * WgResDB::GetSurface( const std::string& id ) const
{
	SurfaceRes* surfRes = GetResSurface(id);
	return surfRes ? surfRes->res : 0;
}

//____ () _________________________________________________________

WgGlyphSet * WgResDB::GetGlyphSet( const std::string& id ) const
{
	GlyphSetRes* glyphRes = GetResGlyphSet(id);
	return glyphRes ? glyphRes->res : 0;
}

//____ () _________________________________________________________

WgFont * WgResDB::GetFont( const std::string& id ) const
{
	FontRes* fontRes = GetResFont(id);
	return fontRes ? fontRes->res : 0;
}

//____ () _________________________________________________________

WgAnim * WgResDB::GetAnim( const std::string& id ) const
{
	AnimRes* animRes = GetResAnim(id);
	return animRes ? animRes->res : 0;
}

//____ () _________________________________________________________

WgCursor * WgResDB::GetCursor( const std::string& id ) const
{
	CursorRes* cursorRes = GetResCursor(id);
	return cursorRes ? cursorRes->res : 0;
}

//____ () _________________________________________________________

WgTextPropPtr WgResDB::GetTextProp( const std::string& id ) const
{
	TextPropRes* propRes = GetResTextProp(id);
	return propRes ? propRes->res : WgTextPropPtr();
}

//____ () _________________________________________________________

WgColor WgResDB::GetColor( const std::string& id ) const
{
	ColorRes* colorRes = GetResColor(id);
	return colorRes ? colorRes->res : WgColor::Black();
}

//____ () _________________________________________________________

WgBlockSetPtr WgResDB::GetBlockSet( const std::string& id ) const
{
	BlockSetRes* blockRes = GetResBlockSet(id);
	return blockRes ? blockRes->res : WgBlockSetPtr();
}

#ifdef WG_LEGACY
//____ () _________________________________________________________

WgWidget * WgResDB::GetWidget( const std::string& id ) const
{
	WidgetRes* widgetRes = GetResWidget(id);
	return widgetRes ? widgetRes->res : 0;
}

//____ () _________________________________________________________

WgItem * WgResDB::GetItem( const std::string& id ) const
{
	ItemRes* itemRes = GetResItem(id);
	return itemRes ? itemRes->res : 0;
}
#endif

//____ () _________________________________________________________

WgMenuItem * WgResDB::GetMenuItem( const std::string& id ) const
{
	MenuItemRes* itemRes = GetResMenuItem(id);
	return itemRes ? itemRes->res : 0;
}

//____ () _________________________________________________________

WgTab * WgResDB::GetTab( const std::string& id ) const
{
	TabRes* itemRes = GetResTab(id);
	return itemRes ? itemRes->res : 0;
}

//____ () _________________________________________________________

WgTextManager * WgResDB::GetTextManager( const std::string& id ) const
{
	TextManagerRes* managerRes = GetResTextManager(id);
	return managerRes ? managerRes->res : 0;
}

//____ () _________________________________________________________

WgSkinManager * WgResDB::GetSkinManager( const std::string& id ) const
{
	SkinManagerRes* managerRes = GetResSkinManager(id);
	return managerRes ? managerRes->res : 0;
}


//____ () _________________________________________________________

WgResDB::ResDBRes * WgResDB::GetResDbRes( const std::string& id ) const
{
	for(ResDBRes* res = GetFirstResDBRes(); res; res = res->Next())
	{
		if(res->file == id)
			return res;
		if(res->res)
		{
			WgResDB::ResDBRes * pDb = res->res->GetResDbRes(id);
			if(pDb)
				return pDb;
		}
	}
	return 0;
}

//____ () _________________________________________________________

WgResDB::SurfaceRes * WgResDB::GetResSurface( const std::string& id ) const
{
	SurfaceRes* res = 0;
	for(ResDBRes* db = GetFirstResDBRes(); db; db = db->Next())
	{
		if(db->res)
		{
			if((res = db->res->GetResSurface(id)))
				return res;
		}
	}
	SurfMap::const_iterator it = m_mapSurfaces.find(id);
	return it == m_mapSurfaces.end() ? 0 : it->second;
}

//____ () _________________________________________________________

WgResDB::GlyphSetRes * WgResDB::GetResGlyphSet( const std::string& id ) const
{
	GlyphSetRes* res = 0;
	for(ResDBRes* db = GetFirstResDBRes(); db; db = db->Next())
	{
		if(db->res)
		{
			if((res = db->res->GetResGlyphSet(id)))
				return res;
		}
	}
	GlyphMap::const_iterator it = m_mapGlyphSets.find(id);
	return it == m_mapGlyphSets.end() ? 0 : it->second;
}

//____ () _________________________________________________________

WgResDB::FontRes * WgResDB::GetResFont( const std::string& id ) const
{
	FontRes* res = 0;
	for(ResDBRes* db = GetFirstResDBRes(); db; db = db->Next())
	{
		if(db->res)
		{
			if((res = db->res->GetResFont(id)))
				return res;
		}
	}
	FontMap::const_iterator it = m_mapFonts.find(id);
	return it == m_mapFonts.end() ? 0 : it->second;
}

//____ () _________________________________________________________

WgResDB::AnimRes * WgResDB::GetResAnim( const std::string& id ) const
{
	AnimRes* res = 0;
	for(ResDBRes* db = GetFirstResDBRes(); db; db = db->Next())
	{
		if(db->res)
		{
			if((res = db->res->GetResAnim(id)))
				return res;
		}
	}
	AnimMap::const_iterator it = m_mapAnims.find(id);
	return it == m_mapAnims.end() ? 0 : it->second;
}

//____ () _________________________________________________________

WgResDB::CursorRes * WgResDB::GetResCursor( const std::string& id ) const
{
	CursorRes* res = 0;
	for(ResDBRes* db = GetFirstResDBRes(); db; db = db->Next())
	{
		if(db->res)
		{
			if((res = db->res->GetResCursor(id)))
				return res;
		}
	}
	CursorMap::const_iterator it = m_mapCursors.find(id);
	return it == m_mapCursors.end() ? 0 : it->second;
}

//____ () _________________________________________________________

WgResDB::TextPropRes * WgResDB::GetResTextProp( const std::string& id ) const
{
	TextPropRes* res = 0;
	for(ResDBRes* db = GetFirstResDBRes(); db; db = db->Next())
	{
		if(db->res)
		{
			if((res = db->res->GetResTextProp(id)))
				return res;
		}
	}
	PropMap::const_iterator it = m_mapTextProps.find(id);
	return it == m_mapTextProps.end() ? 0 : it->second;
}

//____ () _________________________________________________________

WgResDB::ColorRes * WgResDB::GetResColor( const std::string& id ) const
{
	ColorRes* res = 0;
	for(ResDBRes* db = GetFirstResDBRes(); db; db = db->Next())
	{
		if(db->res)
		{
			if((res = db->res->GetResColor(id)))
				return res;
		}
	}
	ColMap::const_iterator it = m_mapColors.find(id);
	return it == m_mapColors.end() ? 0 : it->second;
}

//____ () _________________________________________________________

WgResDB::LegoSource * WgResDB::GetLegoSource( const std::string& id ) const
{
	LegoSource* res = 0;
	for(ResDBRes* db = GetFirstResDBRes(); db; db = db->Next())
	{
		if(db->res)
		{
			if((res = db->res->GetLegoSource(id)))
				return res;
		}
	}
	LegoMap::const_iterator it = m_mapLegoSources.find(id);
	return it == m_mapLegoSources.end() ? 0 : it->second;
}

WgResDB::BlockSetRes * WgResDB::GetResBlockSet( const std::string& id ) const
{
	BlockSetRes* res = 0;
	for(ResDBRes* db = GetFirstResDBRes(); db; db = db->Next())
	{
		if(db->res)
		{
			if((res = db->res->GetResBlockSet(id)))
				return res;
		}
	}
	BlockMap::const_iterator it = m_mapBlockSets.find(id);
	return it == m_mapBlockSets.end() ? 0 : it->second;
}

#ifdef WG_LEGACY
//____ () _________________________________________________________

WgResDB::WidgetRes * WgResDB::GetResWidget( const std::string& id ) const
{
	WidgetRes* res = 0;
	for(ResDBRes* db = GetFirstResDBRes(); db; db = db->Next())
	{
		if(db->res)
		{
			if((res = db->res->GetResWidget(id)))
				return res;
		}
	}
	WdgMap::const_iterator it = m_mapWidgets.find(id);
	return it == m_mapWidgets.end() ? 0 : it->second;
}

//____ () _________________________________________________________

WgResDB::ItemRes * WgResDB::GetResItem( const std::string& id ) const
{
	ItemRes* res = 0;
	for(ResDBRes* db = GetFirstResDBRes(); db; db = db->Next())
	{
		if(db->res)
		{
			if((res = db->res->GetResItem(id)))
				return res;
		}
	}
	ItemMap::const_iterator it = m_mapItems.find(id);
	return it == m_mapItems.end() ? 0 : it->second;
}
#endif

//____ () _________________________________________________________

WgResDB::MenuItemRes * WgResDB::GetResMenuItem( const std::string& id ) const
{
	MenuItemRes* res = 0;
	for(ResDBRes* db = GetFirstResDBRes(); db; db = db->Next())
	{
		if(db->res)
		{
			if((res = db->res->GetResMenuItem(id)))
				return res;
		}
	}
	MenuItemMap::const_iterator it = m_mapMenuItems.find(id);
	return it == m_mapMenuItems.end() ? 0 : it->second;
}

//____ () _________________________________________________________

WgResDB::TabRes * WgResDB::GetResTab( const std::string& id ) const
{
	TabRes* res = 0;
	for(ResDBRes* db = GetFirstResDBRes(); db; db = db->Next())
	{
		if(db->res)
		{
			if((res = db->res->GetResTab(id)))
				return res;
		}
	}
	TabMap::const_iterator it = m_mapTabs.find(id);
	return it == m_mapTabs.end() ? 0 : it->second;
}


//____ () _________________________________________________________

WgResDB::TextManagerRes * WgResDB::GetResTextManager( const std::string& id ) const
{
	TextManagerRes* res = 0;
	for(ResDBRes* db = GetFirstResDBRes(); db; db = db->Next())
	{
		if(db->res)
		{
			if((res = db->res->GetResTextManager(id)))
				return res;
		}
	}
	TextManagerMap::const_iterator it = m_mapTextManagers.find(id);
	return it == m_mapTextManagers.end() ? 0 : it->second;
}

//____ () _________________________________________________________

WgResDB::SkinManagerRes * WgResDB::GetResSkinManager( const std::string& id ) const
{
	SkinManagerRes* res = 0;
	for(ResDBRes* db = GetFirstResDBRes(); db; db = db->Next())
	{
		if(db->res)
		{
			if((res = db->res->GetResSkinManager(id)))
				return res;
		}
	}
	SkinManagerMap::const_iterator it = m_mapSkinManagers.find(id);
	return it == m_mapSkinManagers.end() ? 0 : it->second;
}


//____ () _________________________________________________________

WgResDB::ConnectRes * WgResDB::GetResConnect( const std::string& id ) const
{
	ConnectRes* res = 0;
	for(ResDBRes* db = GetFirstResDBRes(); db; db = db->Next())
	{
		if(db->res)
		{
			if((res = db->res->GetResConnect(id)))
				return res;
		}
	}
	ConnectMap::const_iterator it = m_mapConnects.find(id);
	return it == m_mapConnects.end() ? 0 : it->second;
}

//____ () _________________________________________________________

WgResDB::ResDBRes * WgResDB::FindResDbRes( const WgResDB * data ) const
{
	for(ResDBRes* res = GetFirstResDBRes(); res; res = res->Next())
	{
		if(res->res == data)
			return res;
		if(res->res)
		{
			ResDBRes* res2 = res->res->FindResDbRes(data);
			if(res2)
				return res2;
		}
	}
	return 0;
}

//____ () _________________________________________________________

WgResDB::SurfaceRes* WgResDB::FindResSurface( const WgSurface* surf ) const
{
	SurfaceRes* res = 0;
	for(ResDBRes* db = GetFirstResDBRes(); db; db = db->Next())
	{
		if(db->res)
		{
			if((res = db->res->FindResSurface(surf)))
				return res;
		}
	}
	for(res = GetFirstResSurface(); res; res = res->Next())
		if(res->res == surf)
			return res;
	return 0;
}

//____ () _________________________________________________________

WgResDB::GlyphSetRes* WgResDB::FindResGlyphSet( const WgGlyphSet* meta ) const
{
	GlyphSetRes* res = 0;
	for(ResDBRes* db = GetFirstResDBRes(); db; db = db->Next())
	{
		if(db->res)
		{
			if((res = db->res->FindResGlyphSet(meta)))
				return res;
		}
	}
	for(res = GetFirstResGlyphSet(); res; res = res->Next())
		if(res->res == meta)
			return res;
	return 0;
}

//____ () _________________________________________________________

WgResDB::FontRes* WgResDB::FindResFont( const WgFont* meta ) const
{
	FontRes* res = 0;
	for(ResDBRes* db = GetFirstResDBRes(); db; db = db->Next())
	{
		if(db->res)
		{
			if((res = db->res->FindResFont(meta)))
				return res;
		}
	}
	for(res = GetFirstResFont(); res; res = res->Next())
		if(res->res == meta)
			return res;
	return 0;
}

//____ () _________________________________________________________

WgResDB::AnimRes* WgResDB::FindResAnim( const WgAnim* meta ) const
{
	AnimRes* res = 0;
	for(ResDBRes* db = GetFirstResDBRes(); db; db = db->Next())
	{
		if(db->res)
		{
			if((res = db->res->FindResAnim(meta)))
				return res;
		}
	}
	for(res = GetFirstResAnim(); res; res = res->Next())
		if(res->res == meta)
			return res;
	return 0;
}

//____ () _________________________________________________________

WgResDB::CursorRes* WgResDB::FindResCursor( const WgCursor* meta ) const
{
	CursorRes* res = 0;
	for(ResDBRes* db = GetFirstResDBRes(); db; db = db->Next())
	{
		if(db->res)
		{
			if((res = db->res->FindResCursor(meta)))
				return res;
		}
	}
	for(res = GetFirstResCursor(); res; res = res->Next())
		if(res->res == meta)
			return res;
	return 0;
}

//____ () _________________________________________________________

WgResDB::TextPropRes* WgResDB::FindResTextProp( const WgTextPropPtr meta ) const
{
	TextPropRes* res = 0;
	for(ResDBRes* db = GetFirstResDBRes(); db; db = db->Next())
	{
		if(db->res)
		{
			if((res = db->res->FindResTextProp(meta)))
				return res;
		}
	}
	for(res = GetFirstResTextProp(); res; res = res->Next())
		if(res->res == meta)
			return res;
	return 0;
}

//____ () _________________________________________________________

WgResDB::BlockSetRes* WgResDB::FindResBlockSet( const WgBlockSetPtr meta ) const
{
	BlockSetRes* res = 0;
	for(ResDBRes* db = GetFirstResDBRes(); db; db = db->Next())
	{
		if(db->res)
		{
			if((res = db->res->FindResBlockSet(meta)))
				return res;
		}
	}
	for(res = GetFirstResBlockSet(); res; res = res->Next())
		if(res->res == meta)
			return res;
	return 0;
}

#ifdef WG_LEGACY
//____ () _________________________________________________________

WgResDB::WidgetRes* WgResDB::FindResWidget( const WgWidget* meta ) const
{
	WidgetRes* res = 0;
	for(ResDBRes* db = GetFirstResDBRes(); db; db = db->Next())
	{
		if(db->res)
		{
			if((res = db->res->FindResWidget(meta)))
				return res;
		}
	}
	for(res = GetFirstResWidget(); res; res = res->Next())
		if(res->res == meta)
			return res;
	return 0;
}

//____ () _________________________________________________________

WgResDB::ItemRes* WgResDB::FindResItem( const WgItem* meta ) const
{
	ItemRes * res = 0;
	for(ResDBRes* db = GetFirstResDBRes(); db; db = db->Next())
	{
		if(db->res)
		{
			if((res = db->res->FindResItem(meta)))
				return res;
		}
	}
	for(res = GetFirstResItem(); res; res = res->Next())
		if(res->res == meta)
			return res;
	return 0;
}
#endif

//____ () _________________________________________________________

WgResDB::MenuItemRes* WgResDB::FindResMenuItem( const WgMenuItem* meta ) const
{
	MenuItemRes * res = 0;
	for(ResDBRes* db = GetFirstResDBRes(); db; db = db->Next())
	{
		if(db->res)
		{
			if((res = db->res->FindResMenuItem(meta)))
				return res;
		}
	}
	for(res = GetFirstResMenuItem(); res; res = res->Next())
		if(res->res == meta)
			return res;
	return 0;
}


//____ () _________________________________________________________

WgResDB::TabRes* WgResDB::FindResTab( const WgTab* meta ) const
{
	TabRes * res = 0;
	for(ResDBRes* db = GetFirstResDBRes(); db; db = db->Next())
	{
		if(db->res)
		{
			if((res = db->res->FindResTab(meta)))
				return res;
		}
	}
	for(res = GetFirstResTab(); res; res = res->Next())
		if(res->res == meta)
			return res;
	return 0;
}


//____ () _________________________________________________________

WgResDB::TextManagerRes* WgResDB::FindResTextManager( const WgTextManager* meta ) const
{
	TextManagerRes * res = 0;
	for(ResDBRes* db = GetFirstResDBRes(); db; db = db->Next())
	{
		if(db->res)
		{
			if((res = db->res->FindResTextManager(meta)))
				return res;
		}
	}
	for(res = GetFirstResTextManager(); res; res = res->Next())
		if(res->res == meta)
			return res;
	return 0;
}

//____ () _________________________________________________________

WgResDB::SkinManagerRes* WgResDB::FindResSkinManager( const WgSkinManager* meta ) const
{
	SkinManagerRes * res = 0;
	for(ResDBRes* db = GetFirstResDBRes(); db; db = db->Next())
	{
		if(db->res)
		{
			if((res = db->res->FindResSkinManager(meta)))
				return res;
		}
	}
	for(res = GetFirstResSkinManager(); res; res = res->Next())
		if(res->res == meta)
			return res;
	return 0;
}

//____ RemoveSurface() ________________________________________________________

bool WgResDB::RemoveSurface( const std::string& id )
{
	SurfMap::iterator it = m_mapSurfaces.find( id );

	if( it == m_mapSurfaces.end() )
		return false;

	SurfaceRes * pRes = it->second;
	m_mapSurfaces.erase(it);
	delete pRes;

	return true;
}

bool WgResDB::RemoveSurface( WgResDB::SurfaceRes * pRes )
{
	if( !pRes )
		return false;

	if( pRes->id.length() > 0 )
	{
		SurfMap::iterator it = m_mapSurfaces.find( pRes->id );
		assert( it != m_mapSurfaces.end() );
		m_mapSurfaces.erase(it);
	}
	delete pRes;
	return true;
}


//____ RemoveGlyphSet() _______________________________________________________

bool WgResDB::RemoveGlyphSet( const std::string& id )
{
	GlyphMap::iterator it = m_mapGlyphSets.find( id );

	if( it == m_mapGlyphSets.end() )
		return false;

	GlyphSetRes * pRes = it->second;
	m_mapGlyphSets.erase(it);
	delete pRes;

	return true;
}

bool WgResDB::RemoveGlyphSet( WgResDB::GlyphSetRes * pRes )
{
	if( !pRes )
		return false;

	if( pRes->id.length() > 0 )
	{
		GlyphMap::iterator it = m_mapGlyphSets.find( pRes->id );
		assert( it != m_mapGlyphSets.end() );
		m_mapGlyphSets.erase(it);
	}
	delete pRes;
	return true;
}


//____ RemoveFont() ___________________________________________________________

bool WgResDB::RemoveFont( const std::string& id )
{
	FontMap::iterator it = m_mapFonts.find( id );

	if( it == m_mapFonts.end() )
		return false;

	FontRes * pRes = it->second;
	m_mapFonts.erase(it);
	delete pRes;

	return true;
}

bool WgResDB::RemoveFont( WgResDB::FontRes * pRes )
{
	if( !pRes )
		return false;

	if( pRes->id.length() > 0 )
	{
		FontMap::iterator it = m_mapFonts.find( pRes->id );
		assert( it != m_mapFonts.end() );
		m_mapFonts.erase(it);
	}
	delete pRes;
	return true;
}


//____ RemoveAnim() ___________________________________________________________

bool WgResDB::RemoveAnim( const std::string& id )
{
	AnimMap::iterator it = m_mapAnims.find( id );

	if( it == m_mapAnims.end() )
		return false;

	AnimRes * pRes = it->second;
	m_mapAnims.erase(it);
	delete pRes;

	return true;
}

bool WgResDB::RemoveAnim( WgResDB::AnimRes * pRes )
{
	if( !pRes )
		return false;

	if( pRes->id.length() > 0 )
	{
		AnimMap::iterator it = m_mapAnims.find( pRes->id );
		assert( it != m_mapAnims.end() );
		m_mapAnims.erase(it);
	}
	delete pRes;
	return true;
}


//____ RemoveCursor() ___________________________________________________________

bool WgResDB::RemoveCursor( const std::string& id )
{
	CursorMap::iterator it = m_mapCursors.find( id );

	if( it == m_mapCursors.end() )
		return false;

	CursorRes * pRes = it->second;
	m_mapCursors.erase(it);
	delete pRes;

	return true;
}

bool WgResDB::RemoveCursor( WgResDB::CursorRes * pRes )
{
	if( !pRes )
		return false;

	if( pRes->id.length() > 0 )
	{
		CursorMap::iterator it = m_mapCursors.find( pRes->id );
		assert( it != m_mapCursors.end() );
		m_mapCursors.erase(it);
	}
	delete pRes;
	return true;
}


//____ RemoveTextProp() _______________________________________________________

bool WgResDB::RemoveTextProp( const std::string& id )
{
	PropMap::iterator it = m_mapTextProps.find( id );

	if( it == m_mapTextProps.end() )
		return false;

	TextPropRes * pRes = it->second;
	m_mapTextProps.erase(it);
	delete pRes;

	return true;
}

bool WgResDB::RemoveTextProp( WgResDB::TextPropRes * pRes )
{
	if( !pRes )
		return false;

	if( pRes->id.length() > 0 )
	{
		PropMap::iterator it = m_mapTextProps.find( pRes->id );
		assert( it != m_mapTextProps.end() );
		m_mapTextProps.erase(it);
	}
	delete pRes;
	return true;
}


//____ RemoveColor() __________________________________________________________

bool WgResDB::RemoveColor( const std::string& id )
{
	ColMap::iterator it = m_mapColors.find( id );

	if( it == m_mapColors.end() )
		return false;

	ColorRes * pRes = it->second;
	m_mapColors.erase(it);
	delete pRes;

	return true;
}

bool WgResDB::RemoveColor( WgResDB::ColorRes * pRes )
{
	if( !pRes )
		return false;

	if( pRes->id.length() > 0 )
	{
		ColMap::iterator it = m_mapColors.find( pRes->id );
		assert( it != m_mapColors.end() );
		m_mapColors.erase(it);
	}
	delete pRes;
	return true;
}


//____ RemoveLegoSource() _____________________________________________________

bool WgResDB::RemoveLegoSource( const std::string& id )
{
	LegoMap::iterator it = m_mapLegoSources.find( id );

	if( it == m_mapLegoSources.end() )
		return false;

	LegoSource * pRes = it->second;
	m_mapLegoSources.erase(it);
	delete pRes;
	return true;
}

bool WgResDB::RemoveLegoSource( WgResDB::LegoSource * pRes )
{
	if( !pRes )
		return false;

	if( pRes->id.length() > 0 )
	{
		LegoMap::iterator it = m_mapLegoSources.find( pRes->id );
		assert( it != m_mapLegoSources.end() );
		m_mapLegoSources.erase(it);
	}
	delete pRes;
	return true;
}

//____ RemoveBlockSet() _______________________________________________________

bool WgResDB::RemoveBlockSet( const std::string& id )
{
	BlockMap::iterator it = m_mapBlockSets.find( id );

	if( it == m_mapBlockSets.end() )
		return false;

	BlockSetRes * pRes = it->second;
	m_mapBlockSets.erase(it);
	delete pRes;

	return true;
}

bool WgResDB::RemoveBlockSet( WgResDB::BlockSetRes * pRes )
{
	if( !pRes )
		return false;

	if( pRes->id.length() > 0 )
	{
		BlockMap::iterator it = m_mapBlockSets.find( pRes->id );
		assert( it != m_mapBlockSets.end() );
		m_mapBlockSets.erase(it);
	}
	delete pRes;
	return true;
}

#ifdef WG_LEGACY
//____ RemoveWidget() _________________________________________________________

bool WgResDB::RemoveWidget( const std::string& id )
{
	WdgMap::iterator it = m_mapWidgets.find( id );

	if( it == m_mapWidgets.end() )
		return false;

	WidgetRes * pRes = it->second;
	m_mapWidgets.erase(it);
	delete pRes;

	return true;
}

bool WgResDB::RemoveWidget( WgResDB::WidgetRes * pRes )
{
	if( !pRes )
		return false;

	if( pRes->id.length() > 0 )
	{
		WdgMap::iterator it = m_mapWidgets.find( pRes->id );
		assert( it != m_mapWidgets.end() );
		m_mapWidgets.erase(it);
	}
	delete pRes;
	return true;
}


//____ RemoveItem() ___________________________________________________________

bool WgResDB::RemoveItem( const std::string& id )
{
	ItemMap::iterator it = m_mapItems.find( id );

	if( it == m_mapItems.end() )
		return false;

	ItemRes * pRes = it->second;
	m_mapItems.erase(it);
	delete pRes;

	return true;
}

bool WgResDB::RemoveItem( WgResDB::ItemRes * pRes )
{
	if( !pRes )
		return false;

	if( pRes->id.length() > 0 )
	{
		ItemMap::iterator it = m_mapItems.find( pRes->id );
		assert( it != m_mapItems.end() );
		m_mapItems.erase(it);
	}
	delete pRes;
	return true;
}
#endif

//____ RemoveMenuItem() _______________________________________________________

bool WgResDB::RemoveMenuItem( const std::string& id )
{
	MenuItemMap::iterator it = m_mapMenuItems.find( id );

	if( it == m_mapMenuItems.end() )
		return false;

	MenuItemRes * pRes = it->second;
	m_mapMenuItems.erase(it);
	delete pRes;

	return true;
}

bool WgResDB::RemoveMenuItem( WgResDB::MenuItemRes * pRes )
{
	if( !pRes )
		return false;

	if( pRes->id.length() > 0 )
	{
		MenuItemMap::iterator it = m_mapMenuItems.find( pRes->id );
		assert( it != m_mapMenuItems.end() );
		m_mapMenuItems.erase(it);
	}
	delete pRes;
	return true;
}

//____ RemoveTab() _______________________________________________________

bool WgResDB::RemoveTab( const std::string& id )
{
	TabMap::iterator it = m_mapTabs.find( id );

	if( it == m_mapTabs.end() )
		return false;

	TabRes * pRes = it->second;
	m_mapTabs.erase(it);
	delete pRes;

	return true;
}

bool WgResDB::RemoveTab( WgResDB::TabRes * pRes )
{
	if( !pRes )
		return false;

	if( pRes->id.length() > 0 )
	{
		TabMap::iterator it = m_mapTabs.find( pRes->id );
		assert( it != m_mapTabs.end() );
		m_mapTabs.erase(it);
	}
	delete pRes;
	return true;
}

//____ RemoveTextManager() _______________________________________________________

bool WgResDB::RemoveTextManager( const std::string& id )
{
	TextManagerMap::iterator it = m_mapTextManagers.find( id );

	if( it == m_mapTextManagers.end() )
		return false;

	TextManagerRes * pRes = it->second;
	m_mapTextManagers.erase(it);
	delete pRes;

	return true;
}

bool WgResDB::RemoveTextManager( WgResDB::TextManagerRes * pRes )
{
	if( !pRes )
		return false;

	if( pRes->id.length() > 0 )
	{
		TextManagerMap::iterator it = m_mapTextManagers.find( pRes->id );
		assert( it != m_mapTextManagers.end() );
		m_mapTextManagers.erase(it);
	}
	delete pRes;
	return true;
}

//____ RemoveSkinManager() _______________________________________________________

bool WgResDB::RemoveSkinManager( const std::string& id )
{
	SkinManagerMap::iterator it = m_mapSkinManagers.find( id );

	if( it == m_mapSkinManagers.end() )
		return false;

	SkinManagerRes * pRes = it->second;
	m_mapSkinManagers.erase(it);
	delete pRes;

	return true;
}

bool WgResDB::RemoveSkinManager( WgResDB::SkinManagerRes * pRes )
{
	if( !pRes )
		return false;

	if( pRes->id.length() > 0 )
	{
		SkinManagerMap::iterator it = m_mapSkinManagers.find( pRes->id );
		assert( it != m_mapSkinManagers.end() );
		m_mapSkinManagers.erase(it);
	}
	delete pRes;
	return true;
}


//____ RemoveConnect() _______________________________________________________

bool WgResDB::RemoveConnect( const std::string& id )
{
	ConnectMap::iterator it = m_mapConnects.find( id );

	if( it == m_mapConnects.end() )
		return false;

	ConnectRes * pRes = it->second;
	m_mapConnects.erase(it);
	delete pRes;

	return true;
}

bool WgResDB::RemoveConnect( WgResDB::ConnectRes * pRes )
{
	if( !pRes )
		return false;

	if( pRes->id.length() > 0 )
	{
		ConnectMap::iterator it = m_mapConnects.find( pRes->id );
		assert( it != m_mapConnects.end() );
		m_mapConnects.erase(it);
	}
	delete pRes;
	return true;
}

//____ RemoveResDB() _______________________________________________________

bool WgResDB::RemoveResDB( const std::string& id )
{
	ResDBMap::iterator it = m_mapResDBs.find( id );

	if( it == m_mapResDBs.end() )
		return false;

	ResDBRes * pRes = it->second;
	m_mapResDBs.erase(it);
	delete pRes;

	return true;
}

bool WgResDB::RemoveResDB( WgResDB::ResDBRes * pRes )
{
	if( !pRes )
		return false;

	if( pRes->id.length() > 0 )
	{
		ResDBMap::iterator it = m_mapResDBs.find( pRes->id );
		assert( it != m_mapResDBs.end() );
		m_mapResDBs.erase(it);
	}
	delete pRes;
	return true;
}
