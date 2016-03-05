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

#ifndef WG_SKIN_DOT_H
#	include <wg_skin.h>
#endif

#ifndef WG_GFXANIM_DOT_H
#	include <wg_gfxanim.h>
#endif

#ifndef WG_CARET_DOT_H
#	include <wg_caret2.h>
#endif

#ifndef WG_TEXTSTYLE_DOT_H
#	include <wg_textstyle.h>
#endif

#ifndef WG_GLYPHSET_DOT_H
#	include	<wg_glyphset.h>
#endif

namespace wg 
{
	
	class Font;
	class ResLoader;
	
	
	class ResDB;
	typedef	StrongPtr<ResDB,Object_p>	ResDB_p;
	typedef	WeakPtr<ResDB,Object_wp>	ResDB_wp;
	
	class ResDB : public Object
	{
	public:
		static ResDB_p	create() { return ResDB_p(new ResDB()); }
	
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static ResDB_p	cast( const Object_p& pObject );
	
	
		class MetaData
		{
		public:
			virtual ~MetaData() {}
			virtual MetaData* clone() const = 0;
	
			virtual std::string getAttribute(const std::string& name) const = 0;
			virtual bool		setAttribute(const std::string& name, const std::string& value) = 0;
			virtual bool		hasAttribute(const std::string& name) const = 0;
		};
	
		struct Resource : public Link
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
			LegoSource(const std::string& id, const std::string& surface, Rect rect, uint32_t nStates, MetaData* meta) : Resource(id, meta), surface(surface), rect(rect), nStates(nStates) { }
			std::string surface;
			Rect rect;
			uint32_t nStates;
		};
	
		struct ConnectRes : public Resource
		{
			LINK_METHODS( ConnectRes );
			ConnectRes( MetaData* meta) : Resource("", meta) {}
		};
	
	
		typedef ResWrapper2<ResDB_p>	ResDBRes;
		typedef ResWrapper2<Surface_p>	SurfaceRes;
		typedef ResWrapper2<Glyphset_p>	GlyphsetRes;
		typedef ResWrapper<Font_p>		FontRes;
		typedef ResWrapper<GfxAnim_p>	GfxAnimRes;
		typedef ResWrapper<Caret_p>	CaretRes;
		typedef ResWrapper<TextStyle_p>	TextStyleRes;
		typedef ResWrapper<Color>		ColorRes;
		typedef ResWrapper<Skin_p>		SkinRes;
		typedef ResWrapper<Widget_p>	WidgetRes;
		typedef ResWrapper<void*>		DataSetRes;
	
	
		//----
	
		static std::string	generateName( const Surface_p& data );
		static std::string	generateName( const Glyphset_p& data );
		static std::string	generateName( const Font_p& data );
		static std::string	generateName( const GfxAnim_p& data );
		static std::string	generateName( const Caret_p& data );
		static std::string	generateName( const Color data );
		static std::string	generateName( const TextStyle_p& data );
		static std::string	generateName( const Skin_p& data );
		static std::string	generateName( const Widget_p& data );
	
		void				setResLoader( ResLoader * pLoader );
		void				clear();
	
		void				clearSurfaces();
		void				clearWidgets();
	
		std::string			loadString( const std::string& token );
	
		bool				addResDb( const std::string& file, MetaData * pMetaData = 0 );
		bool				addResDb( const ResDB_p& db, const std::string& file, MetaData * pMetaData = 0 );
	
		bool				addSurface( const std::string& id, const std::string& file, MetaData * pMetaData, bool bRequired );
		bool				addSurface( const std::string& id, const Surface_p& pSurf, const std::string& filename, MetaData * pMetaData = 0 );
	
		bool				addGlyphset( const std::string& id, const std::string& file, MetaData * pMetaData = 0 );
		bool				addGlyphset( const std::string& id, const Glyphset_p& pGlyphset, const std::string& file, MetaData * pMetaData = 0 );
	
		bool				addFont( const std::string& id, const Font_p& pFont, MetaData * pMetaData = 0 );
		bool				addGfxAnim( const std::string& id, const GfxAnim_p& pAnim, MetaData * pMetaData = 0 );
		bool				addCaret( const std::string& id, const Caret_p& pCaret, MetaData * pMetaData = 0 );
		bool				addTextStyle( const std::string& id, const TextStyle_p& pProp, MetaData * pMetaData = 0 );
		bool				addColor( const std::string& id, Color col, MetaData * pMetaData = 0 );
		bool				addLegoSource( const std::string& id, const std::string& surface, Rect rect, uint32_t nStates, MetaData * pMetaData = 0 );
		bool				addSkin( const std::string& id, const Skin_p& pSkin, MetaData * pMetaData = 0 );
		bool				addConnect( MetaData * pMetaData );
		bool				addWidget( const std::string& id, const Widget_p& pWidget, MetaData * pMetaData = 0 );
		bool				addDataSet( const std::string& id, MetaData * pMetaData );
	
		bool				removeSurface( const std::string& id );
		bool				removeGlyphset( const std::string& id );
		bool				removeFont( const std::string& id );
		bool				removeGfxAnim( const std::string& id );
		bool				removeCaret( const std::string& id );
		bool				removeTextStyle( const std::string& id );
		bool				removeColor( const std::string& id );
		bool				removeLegoSource( const std::string& id );
		bool				removeSkin( const std::string& id );
		bool				removeMenuitem( const std::string& id );
		bool				removeTab( const std::string& id );
		bool				removeConnect( const std::string& id );
		bool				removeResDb( const std::string& id );
		bool				removeWidget( const std::string& id );
		bool				removeDataSet( const std::string& id );
	
		bool				removeSurface( SurfaceRes * pRes );
		bool				removeGlyphset( GlyphsetRes * pRes );
		bool				removeFont( FontRes * pRes );
		bool				removeGfxAnim( GfxAnimRes * pRes );
		bool				removeCaret( CaretRes * pRes );
		bool				removeTextStyle( TextStyleRes * pRes );
		bool				removeColor( ColorRes * pRes );
		bool				removeLegoSource( LegoSource * pRes );
		bool				removeSkin( SkinRes * pRes );
		bool				removeConnect( ConnectRes* pRes );
		bool				removeResDb( ResDBRes* pRes );
		bool				removeWidget( WidgetRes * pRes );
		bool				removeDataSet( DataSetRes * pRes );
	
	
	
		Surface_p		getSurface( const std::string& id ) const;
		Glyphset_p		getGlyphset( const std::string& id ) const;
		Font_p			getFont( const std::string& id ) const;
		GfxAnim_p		getGfxAnim( const std::string& id ) const;
		Caret_p			getCaret( const std::string& id ) const;
		TextStyle_p		getTextStyle( const std::string& id ) const;
		Color				getColor( const std::string& id ) const;
		Skin_p			getSkin( const std::string& id ) const;
		Widget_p			getWidget( const std::string& id ) const;
		MetaData*			getDataSet( const std::string& id ) const;
		Widget_p			cloneWidget( const std::string& id ) const;
	
	
		ResDBRes *			getResDbRes( const std::string& id ) const;
		SurfaceRes *		getResSurface( const std::string& id ) const;
		GlyphsetRes *		getResGlyphset( const std::string& id ) const;
		FontRes *			getResFont( const std::string& id ) const;
		GfxAnimRes *		getResGfxAnim( const std::string& id ) const;
		CaretRes *			getResCaret( const std::string& id ) const;
		TextStyleRes *		getResTextStyle( const std::string& id ) const;
		ColorRes *			getResColor( const std::string& id ) const;
		LegoSource *		getLegoSource( const std::string& id ) const;
		SkinRes *			getResSkin( const std::string& id ) const;
		ConnectRes *		getResConnect( const std::string& id ) const;
		WidgetRes *			getResWidget( const std::string& id ) const;
		DataSetRes *		getResDataSet( const std::string& id ) const;
	
		ResDBRes *			findResDbRes( const ResDB_p& data ) const;
		SurfaceRes *		findResSurface( const Surface_p& data ) const;
		GlyphsetRes *		findResGlyphset( const Glyphset_p& data ) const;
		FontRes *			findResFont( const Font_p& data ) const;
		GfxAnimRes *		findResGfxAnim( const GfxAnim_p& data ) const;
		CaretRes *			findResCaret( const Caret_p& data ) const;
		TextStyleRes *		findResTextStyle( const TextStyle_p& data ) const;
		ColorRes *			findResColor( const Color col ) const;
		SkinRes *			findResSkin( const Skin_p& data ) const;
		WidgetRes *			findResWidget( const Widget_p& data ) const;
	
		std::string			findSurfaceId( const Surface_p& data ) const		{ SurfaceRes * r =	findResSurface(data); return r ? r->id : ""; }
		std::string			findGlyphsetId( const Glyphset_p& data ) const		{ GlyphsetRes * r =	findResGlyphset(data); return r ? r->id : ""; }
		std::string			findFontId( const Font_p& data ) const				{ FontRes *	r =		findResFont(data); return r ? r->id : ""; }
		std::string			findGfxAnimId( const GfxAnim_p& data ) const		{ GfxAnimRes *	r =		findResGfxAnim(data); return r ? r->id : ""; }
		std::string			findCaretId( const Caret_p& data ) const			{ CaretRes *	r =	findResCaret(data); return r ? r->id : ""; }
		std::string			findTextStyleId( const TextStyle_p& data ) const	{ TextStyleRes *r =  findResTextStyle(data); return r ? r->id : ""; }
		std::string			findColorId( const Color data ) const				{ ColorRes *r =		findResColor(data); return r ? r->id : ""; }
		std::string			findSkinId( const Skin_p& data ) const			{ SkinRes *r =		findResSkin(data); return r ? r->id : ""; }
		std::string			findWidgetId( const Widget_p& data ) const			{ WidgetRes *r =	findResWidget(data); return r ? r->id : ""; }
	
		inline ResDBRes *		getFirstResDbRes() const { return m_resDbs.first(); }
		inline SurfaceRes *		getFirstResSurface() const { return m_surfaces.first(); }
		inline GlyphsetRes *	getFirstResGlyphset() const { return m_glyphsets.first(); }
		inline FontRes *		getFirstResFont() const { return m_fonts.first(); }
		inline GfxAnimRes *		getFirstResGfxAnim() const { return m_gfxAnims.first(); }
		inline CaretRes *		getFirstResCaret() const { return m_cursors.first(); }
		inline TextStyleRes *	getFirstResTextStyle() const { return m_textProps.first(); }
		inline ColorRes *		getFirstResColor() const { return m_colors.first(); }
		inline SkinRes *		getFirstResSkin() const { return m_skins.first(); }
		inline ConnectRes *		getFirstResConnect() const { return m_connects.first(); }
		inline WidgetRes *		getFirstResWidget() const { return m_widgets.first(); }
		inline DataSetRes *		getFirstResDataSet() const { return m_dataSets.first(); }
	
	
	private:
		ResDB();
		virtual ~ResDB();
	
		typedef std::map<std::string, ResDBRes*>		ResDBMap;
		typedef std::map<std::string, SurfaceRes*>		SurfMap;
		typedef std::map<std::string, GlyphsetRes*>		GlyphMap;
		typedef std::map<std::string, FontRes*>			FontMap;
		typedef std::map<std::string, GfxAnimRes*>		GfxAnimMap;
		typedef std::map<std::string, CaretRes*>		CaretMap;
		typedef std::map<std::string, TextStyleRes*>		PropMap;
		typedef std::map<std::string, ColorRes*>		ColMap;
		typedef std::map<std::string, LegoSource*>		LegoMap;
		typedef std::map<std::string, SkinRes*>			SkinMap;
		typedef std::map<std::string, ConnectRes*>		ConnectMap;
		typedef std::map<std::string, WidgetRes*>		WidgetMap;
		typedef std::map<std::string, DataSetRes*>		DataSetMap;
	
		Chain<ResDBRes>		m_resDbs;
		Chain<SurfaceRes>		m_surfaces;
		Chain<GlyphsetRes>	m_glyphsets;
		Chain<FontRes>		m_fonts;
		Chain<GfxAnimRes>		m_gfxAnims;
		Chain<CaretRes>		m_carets;
		Chain<TextStyleRes>	m_textProps;
		Chain<ColorRes>		m_colors;
		Chain<LegoSource>		m_legos;
		Chain<SkinRes>		m_skins;
		Chain<ConnectRes>		m_connects;
		Chain<WidgetRes>		m_widgets;
		Chain<DataSetRes>		m_dataSets;
	
		ResDBMap		m_mapResDBs;
		ColMap			m_mapColors;
		SurfMap			m_mapSurfaces;
		GlyphMap		m_mapGlyphsets;
		FontMap			m_mapFonts;
		GfxAnimMap		m_mapGfxAnims;
		CaretMap		m_mapCarets;
		PropMap			m_mapTextStyles;
		LegoMap			m_mapLegoSources;
		SkinMap			m_mapSkins;
		ConnectMap		m_mapConnects;
		WidgetMap		m_mapWidgets;
		DataSetMap		m_mapDataSets;
	
		ResLoader *	m_pResLoader;
	};
	
	
	

} // namespace wg
#endif //WG_RESDB_DOT_H
