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
//#include <wg_resources_xml.h>
#include <wg_resloader.h>
#include <assert.h>
#include <wg_font.h>
#include <wg_surface.h>

namespace wg 
{
	
	const char WgResDB::CLASSNAME[] = {"ResDB"};
	
	
	//____ () _________________________________________________________
	
	WgResDB::WgResDB() :
		m_pResLoader(0)
	{
	}
	
	//____ () _________________________________________________________
	
	WgResDB::~WgResDB()
	{
		clear();
	}
	
	//____ isInstanceOf() _________________________________________________________
	
	bool WgResDB::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;
	
		return WgObject::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * WgResDB::className( void ) const
	{
		return CLASSNAME;
	}
	
	//____ cast() _________________________________________________________________
	
	WgResDB_p WgResDB::cast( const WgObject_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return WgResDB_p( static_cast<WgResDB*>(pObject.rawPtr()) );
	
		return 0;
	}
	
	
	//____ () _________________________________________________________
	
	void WgResDB::clear()
	{
		// Clear the quick-reference maps
	
		m_mapColors.clear();
		m_mapSurfaces.clear();
		m_mapGlyphsets.clear();
		m_mapFonts.clear();
		m_mapGfxAnims.clear();
		m_mapCursors.clear();
		m_mapColors.clear();
		m_mapTextprops.clear();
		m_mapLegoSources.clear();
		m_mapSkins.clear();
		m_mapWidgets.clear();
		m_mapConnects.clear();
		m_mapResDBs.clear();
		m_mapDataSets.clear();
	
		// Clear the linked lists, this will also delete the ResWrapper objects
		// along with their meta-data but NOT the resources themselves.
		// This will cause memory leaks until all resources are properly equipped
		// with smartpointers.
	
		m_surfaces.clear();
		m_glyphsets.clear();
		m_fonts.clear();
		m_gfxAnims.clear();
		m_cursors.clear();
		m_textProps.clear();
		m_colors.clear();
		m_legos.clear();
		m_skins.clear();
		m_widgets.clear();
		m_connects.clear();
		m_resDbs.clear();
		m_dataSets.clear();
	}
	
	void WgResDB::clearSurfaces()
	{
		m_mapSurfaces.clear();
		m_surfaces.clear();
	}
	
	void WgResDB::clearWidgets()
	{
		m_mapWidgets.clear();
		m_widgets.clear();
	}
	
	//____ () _________________________________________________________
	
	std::string	WgResDB::generateName( const WgSurface_p& data )
	{
		static int nGenerated = 0;
		char pBuf[100];
		return std::string("_surf__") + WgTextTool::itoa(++nGenerated, pBuf, 10);
	}
	
	std::string	WgResDB::generateName( const WgGlyphset_p& data )
	{
		static int nGenerated = 0;
		char pBuf[100];
		return std::string("_glyphset__") + WgTextTool::itoa(++nGenerated, pBuf, 10);
	}
	
	std::string	WgResDB::generateName( const WgFont_p& data )
	{
		static int nGenerated = 0;
		char pBuf[100];
		return std::string("_font__") + WgTextTool::itoa(++nGenerated, pBuf, 10);
	}
	
	std::string	WgResDB::generateName( const WgGfxAnim_p& data )
	{
		static int nGenerated = 0;
		char pBuf[100];
		return std::string("_anim__") + WgTextTool::itoa(++nGenerated, pBuf, 10);
	}
	
	std::string	WgResDB::generateName( const WgCaret_p& data )
	{
		static int nGenerated = 0;
		char pBuf[100];
		return std::string("_cursor__") + WgTextTool::itoa(++nGenerated, pBuf, 10);
	}
	
	std::string	WgResDB::generateName( const WgColor data )
	{
		static int nGenerated = 0;
		char pBuf[100];
		return std::string("_color__") + WgTextTool::itoa(++nGenerated, pBuf, 10);
	}
	
	std::string	WgResDB::generateName( const WgTextprop_p& data )
	{
		static int nGenerated = 0;
		char pBuf[100];
		return std::string("_textprop__") + WgTextTool::itoa(++nGenerated, pBuf, 10);
	}
	
	std::string	WgResDB::generateName( const WgSkin_p& data )
	{
		static int nGenerated = 0;
		char pBuf[100];
		return std::string("_skin__") + WgTextTool::itoa(++nGenerated, pBuf, 10);
	}
	
	std::string	WgResDB::generateName( const WgWidget_p& data )
	{
		static int nGenerated = 0;
		char pBuf[100];
		return std::string("_widget__") + WgTextTool::itoa(++nGenerated, pBuf, 10);
	}
	
	void WgResDB::setResLoader( WgResLoader * pLoader )
	{
		m_pResLoader = pLoader;
	}
	
	//____ () _________________________________________________________
	bool WgResDB::addResDb( const std::string& file, MetaData * pMetaData )
	{
	//	assert(getResDbRes(file) == 0);
	
		if(getResDbRes(file) == 0 && m_pResLoader)
		{
			WgResDB_p pDb = m_pResLoader->loadDb( file, this );
			// store resource even if load failed. could be an optional include
			ResDBRes* p = new ResDBRes(file, pDb, file, pMetaData);
			m_resDbs.pushBack(p);
			m_mapResDBs[file] = p;
			return true;
		}
		return false;
	}
	
	bool WgResDB::addResDb( const WgResDB_p& db, const std::string& file, MetaData * pMetaData )
	{
		assert(getResDbRes(file) == 0);
	
		if( !db )
			return false;
	
		if(getResDbRes(file) == 0)
		{
			ResDBRes* p = new ResDBRes(file, db, file, pMetaData);
			m_resDbs.pushBack(p);
			if(file.size())
				m_mapResDBs[file] = p;
			return true;
		}
		return false;
	}
	
	bool WgResDB::addSurface( const std::string& id, const std::string& file, MetaData * pMetaData, bool bRequired )
	{
		assert(m_mapSurfaces.find(id) == m_mapSurfaces.end());
	
		if(m_mapSurfaces.find(id) == m_mapSurfaces.end() && m_pResLoader)
		{
			WgSurface_p pSurface = m_pResLoader->loadSurface( file, bRequired );
			if( !pSurface )
				return false;
	
			SurfaceRes* p = new SurfaceRes(id, pSurface, file, pMetaData);
			m_surfaces.pushBack(p);
			if(id.size())
				m_mapSurfaces[id] = p;
			return true;
		}
		return false;
	}
	
	//____ () _________________________________________________________
	
	bool WgResDB::addSurface( const std::string& id, const WgSurface_p& pSurface, const std::string& filename, MetaData * pMetaData )
	{
		assert(m_mapSurfaces.find(id) == m_mapSurfaces.end());
		if(m_mapSurfaces.find(id) == m_mapSurfaces.end())
		{
			SurfaceRes* p = new SurfaceRes(id, pSurface, filename, pMetaData);
			m_surfaces.pushBack(p);
			if(id.size())
				m_mapSurfaces[id] = p;
			return true;
		}
		return false;
	}
	
	
	//____ () _________________________________________________________
	
	bool WgResDB::addGlyphset( const std::string& id, const std::string& file, MetaData * pMetaData )
	{
		assert(m_mapGlyphsets.find(id) == m_mapGlyphsets.end());
	
		if(m_mapGlyphsets.find(id) == m_mapGlyphsets.end() && m_pResLoader)
		{
			WgGlyphset_p pGlyphset = m_pResLoader->loadGlyphset( file );
			if( !pGlyphset )
				return false;
	
			GlyphsetRes* p = new GlyphsetRes(id, pGlyphset, file, pMetaData);
			m_glyphsets.pushBack(p);
			if(id.size())
				m_mapGlyphsets[id] = p;
			return true;
		}
		return false;
	}
	
	bool WgResDB::addGlyphset( const std::string& id, const WgGlyphset_p& pGlyphset, const std::string& file, MetaData * pMetaData )
	{
		assert(m_mapGlyphsets.find(id) == m_mapGlyphsets.end());
		if(m_mapGlyphsets.find(id) == m_mapGlyphsets.end())
		{
			GlyphsetRes* p = new GlyphsetRes(id, pGlyphset, file, pMetaData);
			m_glyphsets.pushBack(p);
			if(id.size())
				m_mapGlyphsets[id] = p;
			return true;
		}
		return false;
	}
	
	//____ () _________________________________________________________
	
	bool WgResDB::addFont( const std::string& id, const WgFont_p& pFont, MetaData * pMetaData )
	{
		assert(m_mapFonts.find(id) == m_mapFonts.end());
		if(m_mapFonts.find(id) == m_mapFonts.end())
		{
			FontRes* p = new FontRes(id, pFont, pMetaData);
			m_fonts.pushBack(p);
			if(id.size())
				m_mapFonts[id] = p;
			return true;
		}
		return false;
	}
	
	//____ () _________________________________________________________
	
	bool WgResDB::addGfxAnim( const std::string& id, const WgGfxAnim_p& pAnim, MetaData * pMetaData )
	{
		assert(m_mapGfxAnims.find(id) == m_mapGfxAnims.end());
		if(m_mapGfxAnims.find(id) == m_mapGfxAnims.end())
		{
			GfxAnimRes* p = new GfxAnimRes(id, pAnim, pMetaData);
			m_gfxAnims.pushBack(p);
			if(id.size())
				m_mapGfxAnims[id] = p;
			return true;
		}
		return false;
	}
	
	//____ () _________________________________________________________
	
	bool WgResDB::addCursor( const std::string& id, const WgCaret_p& pCursor, MetaData * pMetaData )
	{
		assert(m_mapCursors.find(id) == m_mapCursors.end());
		if(m_mapCursors.find(id) == m_mapCursors.end())
		{
			CursorRes* p = new CursorRes(id, pCursor, pMetaData);
			m_cursors.pushBack(p);
			if(id.size())
				m_mapCursors[id] = p;
			return true;
		}
		return false;
	}
	
	//____ () _________________________________________________________
	
	bool WgResDB::addTextprop( const std::string& id, const WgTextprop_p& pProp, MetaData * pMetaData )
	{
		//assert(m_mapTextprops.find(id) == m_mapTextprops.end());
		if(m_mapTextprops.find(id) == m_mapTextprops.end())
		{
			TextpropRes* p = new TextpropRes(id, pProp, pMetaData);
			m_textProps.pushBack(p);
			if(id.size())
				m_mapTextprops[id] = p;
			return true;
		}
		return false;
	}
	
	//____ () _________________________________________________________
	
	bool WgResDB::addColor( const std::string& id, WgColor col, MetaData * pMetaData )
	{
		assert(m_mapColors.find(id) == m_mapColors.end());
		if(m_mapColors.find(id) == m_mapColors.end())
		{
			ColorRes* p = new ColorRes(id, col, pMetaData);
			m_colors.pushBack(p);
			if(id.size())
				m_mapColors[id] = p;
			return true;
		}
		return false;
	}
	
	//____ () _________________________________________________________
	
	bool WgResDB::addLegoSource( const std::string& id, const std::string& surface, WgRect rect, Uint32 nStates, MetaData * pMetaData )
	{
		assert(m_mapLegoSources.find(id) == m_mapLegoSources.end());
		if(m_mapLegoSources.find(id) == m_mapLegoSources.end())
		{
			LegoSource* p = new LegoSource(id, surface, rect, nStates, pMetaData);
			m_legos.pushBack(p);
			if(id.size())
				m_mapLegoSources[id] = p;
			return true;
		}
		return false;
	}
	
	std::string WgResDB::loadString( const std::string& token )
	{
		return m_pResLoader->loadString(token);
	}
	
	//____ () _________________________________________________________
	
	bool WgResDB::addSkin( const std::string& id, const WgSkin_p& pSkin, MetaData * pMetaData )
	{
		assert(m_mapSkins.find(id) == m_mapSkins.end());
		if(m_mapSkins.find(id) == m_mapSkins.end())
		{
			SkinRes* p = new SkinRes(id, pSkin, pMetaData);
			m_skins.pushBack(p);
			if(id.size())
				m_mapSkins[id] = p;
			return true;
		}
		return false;
	}
	
	//____ () _________________________________________________________
	
	bool WgResDB::addDataSet( const std::string& id, MetaData * pMetaData )
	{
		assert(m_mapDataSets.find(id) == m_mapDataSets.end());
		if(m_mapDataSets.find(id) == m_mapDataSets.end())
		{
			DataSetRes* p = new DataSetRes(id, 0, pMetaData);
			m_dataSets.pushBack(p);
			if(id.size())
				m_mapDataSets[id] = p;
			return true;
		}
		return false;
	}
	
	
	//____ () _________________________________________________________
	
	bool WgResDB::addWidget( const std::string& id, const WgWidget_p& pWidget, MetaData * pMetaData )
	{
		assert(m_mapWidgets.find(id) == m_mapWidgets.end());
		if(m_mapWidgets.find(id) == m_mapWidgets.end())
		{
			WidgetRes* p = new WidgetRes(id, pWidget, pMetaData);
			m_widgets.pushBack(p);
			if(id.size())
				m_mapWidgets[id] = p;
			return true;
		}
		return false;
	}
	
	
	//____ () _________________________________________________________
	
	bool WgResDB::addConnect( MetaData * pMetaData )
	{
		ConnectRes* p = new ConnectRes(pMetaData);
		m_connects.pushBack(p);
		return true;
	}
	
	//____ () _________________________________________________________
	
	WgSurface_p WgResDB::getSurface( const std::string& id ) const
	{
		SurfaceRes* surfRes = getResSurface(id);
		return surfRes ? surfRes->res : WgSurface_p();
	}
	
	//____ () _________________________________________________________
	
	WgGlyphset_p WgResDB::getGlyphset( const std::string& id ) const
	{
		GlyphsetRes* glyphRes = getResGlyphset(id);
		return glyphRes ? glyphRes->res : WgGlyphset_p();
	}
	
	//____ () _________________________________________________________
	
	WgFont_p WgResDB::getFont( const std::string& id ) const
	{
		FontRes* fontRes = getResFont(id);
		return fontRes ? fontRes->res : WgFont_p();
	}
	
	//____ () _________________________________________________________
	
	WgGfxAnim_p WgResDB::getGfxAnim( const std::string& id ) const
	{
		GfxAnimRes* animRes = getResGfxAnim(id);
		return animRes ? animRes->res : WgGfxAnim_p();
	}
	
	//____ () _________________________________________________________
	
	WgCaret_p WgResDB::getCursor( const std::string& id ) const
	{
		CursorRes* cursorRes = getResCursor(id);
		return cursorRes ? cursorRes->res : WgCaret_p();
	}
	
	//____ () _________________________________________________________
	
	WgTextprop_p WgResDB::getTextprop( const std::string& id ) const
	{
		TextpropRes* propRes = getResTextprop(id);
		return propRes ? propRes->res : WgTextprop_p();
	}
	
	//____ () _________________________________________________________
	
	WgColor WgResDB::getColor( const std::string& id ) const
	{
		ColorRes* colorRes = getResColor(id);
		return colorRes ? colorRes->res : WgColor::black;
	}
	
	//____ () _________________________________________________________
	
	WgSkin_p WgResDB::getSkin( const std::string& id ) const
	{
		SkinRes* skinRes = getResSkin(id);
		return skinRes ? skinRes->res : WgSkin_p();
	}
	
	//____ () _________________________________________________________
	
	WgResDB::MetaData * WgResDB::getDataSet( const std::string& id ) const
	{
		DataSetRes* dataSetRes = getResDataSet(id);
		return dataSetRes ? dataSetRes->meta : 0;
	}
	
	//____ () _________________________________________________________
	
	WgWidget_p WgResDB::getWidget( const std::string& id ) const
	{
		WidgetRes* widgetRes = getResWidget(id);
		return widgetRes ? widgetRes->res : WgWidget_p();
	}
	
	//____ () _________________________________________________________
	
	WgWidget_p WgResDB::cloneWidget( const std::string& id ) const
	{
		WidgetRes* widgetRes = getResWidget(id);
	
		if( !widgetRes )
			return 0;
	
		WgWidget_p pWidget = widgetRes->res;
	
		WgWidget_p pClone = pWidget->NewOfMyType();
		pClone->CloneContent(pWidget.rawPtr());
		return pClone;
	}
	
	//____ () _________________________________________________________
	
	WgResDB::ResDBRes * WgResDB::getResDbRes( const std::string& id ) const
	{
		for(ResDBRes* res = getFirstResDbRes(); res; res = res->next())
		{
			if(res->file == id)
				return res;
			if(res->res)
			{
				WgResDB::ResDBRes * pDb = res->res->getResDbRes(id);
				if(pDb)
					return pDb;
			}
		}
		return 0;
	}
	
	//____ () _________________________________________________________
	
	WgResDB::SurfaceRes * WgResDB::getResSurface( const std::string& id ) const
	{
		SurfaceRes* res = 0;
		for(ResDBRes* db = getFirstResDbRes(); db; db = db->next())
		{
			if(db->res)
			{
				if((res = db->res->getResSurface(id)))
					return res;
			}
		}
		SurfMap::const_iterator it = m_mapSurfaces.find(id);
		return it == m_mapSurfaces.end() ? 0 : it->second;
	}
	
	//____ () _________________________________________________________
	
	WgResDB::GlyphsetRes * WgResDB::getResGlyphset( const std::string& id ) const
	{
		GlyphsetRes* res = 0;
		for(ResDBRes* db = getFirstResDbRes(); db; db = db->next())
		{
			if(db->res)
			{
				if((res = db->res->getResGlyphset(id)))
					return res;
			}
		}
		GlyphMap::const_iterator it = m_mapGlyphsets.find(id);
		return it == m_mapGlyphsets.end() ? 0 : it->second;
	}
	
	//____ () _________________________________________________________
	
	WgResDB::FontRes * WgResDB::getResFont( const std::string& id ) const
	{
		FontRes* res = 0;
		for(ResDBRes* db = getFirstResDbRes(); db; db = db->next())
		{
			if(db->res)
			{
				if((res = db->res->getResFont(id)))
					return res;
			}
		}
		FontMap::const_iterator it = m_mapFonts.find(id);
		return it == m_mapFonts.end() ? 0 : it->second;
	}
	
	//____ () _________________________________________________________
	
	WgResDB::GfxAnimRes * WgResDB::getResGfxAnim( const std::string& id ) const
	{
		GfxAnimRes* res = 0;
		for(ResDBRes* db = getFirstResDbRes(); db; db = db->next())
		{
			if(db->res)
			{
				if((res = db->res->getResGfxAnim(id)))
					return res;
			}
		}
		GfxAnimMap::const_iterator it = m_mapGfxAnims.find(id);
		return it == m_mapGfxAnims.end() ? 0 : it->second;
	}
	
	//____ () _________________________________________________________
	
	WgResDB::CursorRes * WgResDB::getResCursor( const std::string& id ) const
	{
		CursorRes* res = 0;
		for(ResDBRes* db = getFirstResDbRes(); db; db = db->next())
		{
			if(db->res)
			{
				if((res = db->res->getResCursor(id)))
					return res;
			}
		}
		CursorMap::const_iterator it = m_mapCursors.find(id);
		return it == m_mapCursors.end() ? 0 : it->second;
	}
	
	//____ () _________________________________________________________
	
	WgResDB::TextpropRes * WgResDB::getResTextprop( const std::string& id ) const
	{
		TextpropRes* res = 0;
		for(ResDBRes* db = getFirstResDbRes(); db; db = db->next())
		{
			if(db->res)
			{
				if((res = db->res->getResTextprop(id)))
					return res;
			}
		}
		PropMap::const_iterator it = m_mapTextprops.find(id);
		return it == m_mapTextprops.end() ? 0 : it->second;
	}
	
	//____ () _________________________________________________________
	
	WgResDB::ColorRes * WgResDB::getResColor( const std::string& id ) const
	{
		ColorRes* res = 0;
		for(ResDBRes* db = getFirstResDbRes(); db; db = db->next())
		{
			if(db->res)
			{
				if((res = db->res->getResColor(id)))
					return res;
			}
		}
		ColMap::const_iterator it = m_mapColors.find(id);
		return it == m_mapColors.end() ? 0 : it->second;
	}
	
	
	//____ () _________________________________________________________
	
	WgResDB::LegoSource * WgResDB::getLegoSource( const std::string& id ) const
	{
		LegoSource* res = 0;
		for(ResDBRes* db = getFirstResDbRes(); db; db = db->next())
		{
			if(db->res)
			{
				if((res = db->res->getLegoSource(id)))
					return res;
			}
		}
		LegoMap::const_iterator it = m_mapLegoSources.find(id);
		return it == m_mapLegoSources.end() ? 0 : it->second;
	}
	
	WgResDB::SkinRes * WgResDB::getResSkin( const std::string& id ) const
	{
		SkinRes* res = 0;
		for(ResDBRes* db = getFirstResDbRes(); db; db = db->next())
		{
			if(db->res)
			{
				if((res = db->res->getResSkin(id)))
					return res;
			}
		}
		SkinMap::const_iterator it = m_mapSkins.find(id);
		return it == m_mapSkins.end() ? 0 : it->second;
	}
	
	//____ () _________________________________________________________
	
	WgResDB::WidgetRes * WgResDB::getResWidget( const std::string& id ) const
	{
		WidgetRes* res = 0;
		for(ResDBRes* db = getFirstResDbRes(); db; db = db->next())
		{
			if(db->res)
			{
				if((res = db->res->getResWidget(id)))
					return res;
			}
		}
		WidgetMap::const_iterator it = m_mapWidgets.find(id);
		return it == m_mapWidgets.end() ? 0 : it->second;
	}
	
	//____ () _________________________________________________________
	
	WgResDB::DataSetRes * WgResDB::getResDataSet( const std::string& id ) const
	{
		DataSetRes* res = 0;
		for(ResDBRes* db = getFirstResDbRes(); db; db = db->next())
		{
			if(db->res)
			{
				if((res = db->res->getResDataSet(id)))
					return res;
			}
		}
		DataSetMap::const_iterator it = m_mapDataSets.find(id);
		return it == m_mapDataSets.end() ? 0 : it->second;
	}
	
	//____ () _________________________________________________________
	
	WgResDB::ConnectRes * WgResDB::getResConnect( const std::string& id ) const
	{
		ConnectRes* res = 0;
		for(ResDBRes* db = getFirstResDbRes(); db; db = db->next())
		{
			if(db->res)
			{
				if((res = db->res->getResConnect(id)))
					return res;
			}
		}
		ConnectMap::const_iterator it = m_mapConnects.find(id);
		return it == m_mapConnects.end() ? 0 : it->second;
	}
	
	//____ () _________________________________________________________
	
	WgResDB::ResDBRes * WgResDB::findResDbRes( const WgResDB_p& data ) const
	{
		for(ResDBRes* res = getFirstResDbRes(); res; res = res->next())
		{
			if(res->res == data)
				return res;
			if(res->res)
			{
				ResDBRes* res2 = res->res->findResDbRes(data);
				if(res2)
					return res2;
			}
		}
		return 0;
	}
	
	//____ () _________________________________________________________
	
	WgResDB::SurfaceRes* WgResDB::findResSurface( const WgSurface_p& surf ) const
	{
		SurfaceRes* res = 0;
		for(ResDBRes* db = getFirstResDbRes(); db; db = db->next())
		{
			if(db->res)
			{
				if((res = db->res->findResSurface(surf)))
					return res;
			}
		}
		for(res = getFirstResSurface(); res; res = res->next())
			if(res->res == surf)
				return res;
		return 0;
	}
	
	//____ () _________________________________________________________
	
	WgResDB::GlyphsetRes* WgResDB::findResGlyphset( const WgGlyphset_p& meta ) const
	{
		GlyphsetRes* res = 0;
		for(ResDBRes* db = getFirstResDbRes(); db; db = db->next())
		{
			if(db->res)
			{
				if((res = db->res->findResGlyphset(meta)))
					return res;
			}
		}
		for(res = getFirstResGlyphset(); res; res = res->next())
			if(res->res == meta)
				return res;
		return 0;
	}
	
	//____ () _________________________________________________________
	
	WgResDB::FontRes* WgResDB::findResFont( const WgFont_p& meta ) const
	{
		FontRes* res = 0;
		for(ResDBRes* db = getFirstResDbRes(); db; db = db->next())
		{
			if(db->res)
			{
				if((res = db->res->findResFont(meta)))
					return res;
			}
		}
		for(res = getFirstResFont(); res; res = res->next())
			if(res->res == meta)
				return res;
		return 0;
	}
	
	//____ () _________________________________________________________
	
	WgResDB::GfxAnimRes* WgResDB::findResGfxAnim( const WgGfxAnim_p& meta ) const
	{
		GfxAnimRes* res = 0;
		for(ResDBRes* db = getFirstResDbRes(); db; db = db->next())
		{
			if(db->res)
			{
				if((res = db->res->findResGfxAnim(meta)))
					return res;
			}
		}
		for(res = getFirstResGfxAnim(); res; res = res->next())
			if(res->res == meta)
				return res;
		return 0;
	}
	
	//____ () _________________________________________________________
	
	WgResDB::CursorRes* WgResDB::findResCursor( const WgCaret_p& meta ) const
	{
		CursorRes* res = 0;
		for(ResDBRes* db = getFirstResDbRes(); db; db = db->next())
		{
			if(db->res)
			{
				if((res = db->res->findResCursor(meta)))
					return res;
			}
		}
		for(res = getFirstResCursor(); res; res = res->next())
			if(res->res == meta)
				return res;
		return 0;
	}
	
	//____ () _________________________________________________________
	
	WgResDB::TextpropRes* WgResDB::findResTextprop( const WgTextprop_p& meta ) const
	{
		TextpropRes* res = 0;
		for(ResDBRes* db = getFirstResDbRes(); db; db = db->next())
		{
			if(db->res)
			{
				if((res = db->res->findResTextprop(meta)))
					return res;
			}
		}
		for(res = getFirstResTextprop(); res; res = res->next())
			if(res->res == meta)
				return res;
		return 0;
	}
	
	//____ () _________________________________________________________
	
	WgResDB::ColorRes* WgResDB::findResColor( const WgColor meta ) const
	{
		ColorRes* res = 0;
		for(ResDBRes* db = getFirstResDbRes(); db; db = db->next())
		{
			if(db->res)
			{
				if((res = db->res->findResColor(meta)))
					return res;
			}
		}
		for(res = getFirstResColor(); res; res = res->next())
			if(res->res == meta)
				return res;
		return 0;
	}
	
	//____ () _________________________________________________________
	
	WgResDB::SkinRes* WgResDB::findResSkin( const WgSkin_p& meta ) const
	{
		SkinRes* res = 0;
		for(ResDBRes* db = getFirstResDbRes(); db; db = db->next())
		{
			if(db->res)
			{
				if((res = db->res->findResSkin(meta)))
					return res;
			}
		}
		for(res = getFirstResSkin(); res; res = res->next())
			if(res->res == meta)
				return res;
		return 0;
	}
	
	//____ () _________________________________________________________
	
	WgResDB::WidgetRes* WgResDB::findResWidget( const WgWidget_p& meta ) const
	{
		WidgetRes* res = 0;
		for(ResDBRes* db = getFirstResDbRes(); db; db = db->next())
		{
			if(db->res)
			{
				if((res = db->res->findResWidget(meta)))
					return res;
			}
		}
		for(res = getFirstResWidget(); res; res = res->next())
			if(res->res == meta)
				return res;
		return 0;
	}
	
	
	//____ removeSurface() ________________________________________________________
	
	bool WgResDB::removeSurface( const std::string& id )
	{
		SurfMap::iterator it = m_mapSurfaces.find( id );
	
		if( it == m_mapSurfaces.end() )
			return false;
	
		SurfaceRes * pRes = it->second;
		m_mapSurfaces.erase(it);
		delete pRes;
	
		return true;
	}
	
	bool WgResDB::removeSurface( WgResDB::SurfaceRes * pRes )
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
	
	
	//____ removeGlyphset() _______________________________________________________
	
	bool WgResDB::removeGlyphset( const std::string& id )
	{
		GlyphMap::iterator it = m_mapGlyphsets.find( id );
	
		if( it == m_mapGlyphsets.end() )
			return false;
	
		GlyphsetRes * pRes = it->second;
		m_mapGlyphsets.erase(it);
		delete pRes;
	
		return true;
	}
	
	bool WgResDB::removeGlyphset( WgResDB::GlyphsetRes * pRes )
	{
		if( !pRes )
			return false;
	
		if( pRes->id.length() > 0 )
		{
			GlyphMap::iterator it = m_mapGlyphsets.find( pRes->id );
			assert( it != m_mapGlyphsets.end() );
			m_mapGlyphsets.erase(it);
		}
		delete pRes;
		return true;
	}
	
	
	//____ removeFont() ___________________________________________________________
	
	bool WgResDB::removeFont( const std::string& id )
	{
		FontMap::iterator it = m_mapFonts.find( id );
	
		if( it == m_mapFonts.end() )
			return false;
	
		FontRes * pRes = it->second;
		m_mapFonts.erase(it);
		delete pRes;
	
		return true;
	}
	
	bool WgResDB::removeFont( WgResDB::FontRes * pRes )
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
	
	bool WgResDB::removeGfxAnim( const std::string& id )
	{
		GfxAnimMap::iterator it = m_mapGfxAnims.find( id );
	
		if( it == m_mapGfxAnims.end() )
			return false;
	
		GfxAnimRes * pRes = it->second;
		m_mapGfxAnims.erase(it);
		delete pRes;
	
		return true;
	}
	
	bool WgResDB::removeGfxAnim( WgResDB::GfxAnimRes * pRes )
	{
		if( !pRes )
			return false;
	
		if( pRes->id.length() > 0 )
		{
			GfxAnimMap::iterator it = m_mapGfxAnims.find( pRes->id );
			assert( it != m_mapGfxAnims.end() );
			m_mapGfxAnims.erase(it);
		}
		delete pRes;
		return true;
	}
	
	
	//____ removeCursor() ___________________________________________________________
	
	bool WgResDB::removeCursor( const std::string& id )
	{
		CursorMap::iterator it = m_mapCursors.find( id );
	
		if( it == m_mapCursors.end() )
			return false;
	
		CursorRes * pRes = it->second;
		m_mapCursors.erase(it);
		delete pRes;
	
		return true;
	}
	
	bool WgResDB::removeCursor( WgResDB::CursorRes * pRes )
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
	
	
	//____ removeTextprop() _______________________________________________________
	
	bool WgResDB::removeTextprop( const std::string& id )
	{
		PropMap::iterator it = m_mapTextprops.find( id );
	
		if( it == m_mapTextprops.end() )
			return false;
	
		TextpropRes * pRes = it->second;
		m_mapTextprops.erase(it);
		delete pRes;
	
		return true;
	}
	
	bool WgResDB::removeTextprop( WgResDB::TextpropRes * pRes )
	{
		if( !pRes )
			return false;
	
		if( pRes->id.length() > 0 )
		{
			PropMap::iterator it = m_mapTextprops.find( pRes->id );
			assert( it != m_mapTextprops.end() );
			m_mapTextprops.erase(it);
		}
		delete pRes;
		return true;
	}
	
	
	//____ removeColor() __________________________________________________________
	
	bool WgResDB::removeColor( const std::string& id )
	{
		ColMap::iterator it = m_mapColors.find( id );
	
		if( it == m_mapColors.end() )
			return false;
	
		ColorRes * pRes = it->second;
		m_mapColors.erase(it);
		delete pRes;
	
		return true;
	}
	
	bool WgResDB::removeColor( WgResDB::ColorRes * pRes )
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
	
	
	
	//____ removeLegoSource() _____________________________________________________
	
	bool WgResDB::removeLegoSource( const std::string& id )
	{
		LegoMap::iterator it = m_mapLegoSources.find( id );
	
		if( it == m_mapLegoSources.end() )
			return false;
	
		LegoSource * pRes = it->second;
		m_mapLegoSources.erase(it);
		delete pRes;
		return true;
	}
	
	bool WgResDB::removeLegoSource( WgResDB::LegoSource * pRes )
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
	
	//____ removeSkin() _______________________________________________________
	
	bool WgResDB::removeSkin( const std::string& id )
	{
		SkinMap::iterator it = m_mapSkins.find( id );
	
		if( it == m_mapSkins.end() )
			return false;
	
		SkinRes * pRes = it->second;
		m_mapSkins.erase(it);
		delete pRes;
	
		return true;
	}
	
	bool WgResDB::removeSkin( WgResDB::SkinRes * pRes )
	{
		if( !pRes )
			return false;
	
		if( pRes->id.length() > 0 )
		{
			SkinMap::iterator it = m_mapSkins.find( pRes->id );
			assert( it != m_mapSkins.end() );
			m_mapSkins.erase(it);
		}
		delete pRes;
		return true;
	}
	
	//____ removeDataSet() _________________________________________________________
	
	bool WgResDB::removeDataSet( const std::string& id )
	{
		DataSetMap::iterator it = m_mapDataSets.find( id );
	
		if( it == m_mapDataSets.end() )
			return false;
	
		DataSetRes * pRes = it->second;
		m_mapDataSets.erase(it);
		delete pRes;
	
		return true;
	}
	
	bool WgResDB::removeDataSet( WgResDB::DataSetRes * pRes )
	{
		if( !pRes )
			return false;
	
		if( pRes->id.length() > 0 )
		{
			DataSetMap::iterator it = m_mapDataSets.find( pRes->id );
			assert( it != m_mapDataSets.end() );
			m_mapDataSets.erase(it);
		}
		delete pRes;
		return true;
	}
	
	
	//____ removeWidget() _________________________________________________________
	
	bool WgResDB::removeWidget( const std::string& id )
	{
		WidgetMap::iterator it = m_mapWidgets.find( id );
	
		if( it == m_mapWidgets.end() )
			return false;
	
		WidgetRes * pRes = it->second;
		m_mapWidgets.erase(it);
		delete pRes;
	
		return true;
	}
	
	bool WgResDB::removeWidget( WgResDB::WidgetRes * pRes )
	{
		if( !pRes )
			return false;
	
		if( pRes->id.length() > 0 )
		{
			WidgetMap::iterator it = m_mapWidgets.find( pRes->id );
			assert( it != m_mapWidgets.end() );
			m_mapWidgets.erase(it);
		}
		delete pRes;
		return true;
	}
	
	
	//____ removeConnect() _______________________________________________________
	
	bool WgResDB::removeConnect( const std::string& id )
	{
		ConnectMap::iterator it = m_mapConnects.find( id );
	
		if( it == m_mapConnects.end() )
			return false;
	
		ConnectRes * pRes = it->second;
		m_mapConnects.erase(it);
		delete pRes;
	
		return true;
	}
	
	bool WgResDB::removeConnect( WgResDB::ConnectRes * pRes )
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
	
	//____ removeResDb() _______________________________________________________
	
	bool WgResDB::removeResDb( const std::string& id )
	{
		ResDBMap::iterator it = m_mapResDBs.find( id );
	
		if( it == m_mapResDBs.end() )
			return false;
	
		ResDBRes * pRes = it->second;
		m_mapResDBs.erase(it);
		delete pRes;
	
		return true;
	}
	
	bool WgResDB::removeResDb( WgResDB::ResDBRes * pRes )
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

} // namespace wg
