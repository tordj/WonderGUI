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
#ifndef WG_SELECTBOX_DOT_H
#define WG_SELECTBOX_DOT_H
#pragma once


#include <wg_widget.h>
#include <wg_sidecanvas.h>
#include <wg_ctextdisplay.h>
#include <wg_cdynamicvector.h>

namespace wg
{

	class SelectBox;
	
	//____ SelectBoxEntry ________________________________________________________

	class SelectBoxEntry : private Text
	{
		friend class SelectBox;
	public:
		SelectBoxEntry( int id, const String& text ) : m_id(id), m_string(text) {};
		
		inline int		id() const { return m_id; }
		inline String	text() const { return m_string; }

	protected:

		// Overloaded from Text

		SizeSPX		_textSize() const override;
		State		_textState() const override;
		TextStyle * _textStyle() const override;

		const Char *_textBegin() const override;
		int 		_textLength() const override;

		bool		_caretVisible() const override;
		int			_caretOffset() const override;
		std::tuple<int, int>	_selectedText() const override;

		void		_mapperRequestRender() override;
		void		_mapperRequestRender(const RectSPX& rect) override;
		void		_mapperRequestResize() override;

		int			_scale() const override;

		int			m_id;
		spx			m_height;
		String 		m_string;
		SelectBox * m_pParent = nullptr;
		State		m_state = StateEnum::Normal;
	};
		

	//____ SelectBox __________________________________________________________

	class SelectBox;
	typedef	StrongPtr<SelectBox>		SelectBox_p;
	typedef	WeakPtr<SelectBox>		SelectBox_wp;


	class SelectBox : public Widget, protected CDynamicVector<SelectBoxEntry>::Holder, protected SideCanvas::Holder
	{
		friend class SelectBoxEntry;
	public:
		//.____ Creation __________________________________________

		static SelectBox_p	create() { return SelectBox_p(new SelectBox()); }

		//.____ Components ____________________________________

		CTextDisplay			text;
		CDynamicVector<SelectBoxEntry>	entries;

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Appearance ________________________________________________________

		void			setEntrySkin(Skin * pSkin);
		Skin_p			entrySkin() const { return m_pEntrySkin;  }

		void			setEntryStyle(TextStyle * pStyle);
		TextStyle_p		entryStyle() const { return m_pEntryStyle; }

		void			setListTextLayout(TextLayout * pTextLayout);
		TextLayout_p	listTextLayout() const { return m_pListTextLayout; }

		void			setListSkin(Skin * pSkin);
		Skin_p			listSkin() const { return m_pListCanvas->skin(); }

		//.___ Control _____________________________________________________________

		void			selectEntry(const CDynamicVector<SelectBoxEntry>::iterator& it);
		bool			selectEntryById(int id);
		void			selectEntryByIndex(int index);

		int				selectedEntryId() const;
		inline int		selectedEntryIndex() const { return m_selectedEntryIndex; }

		//.____ Internal ______________________________________________________

		SizeSPX		_defaultSize(int scale = -1) const override;
		spx			_matchingHeight(spx width, int scale = -1) const override;

	protected:
		SelectBox();
		virtual ~SelectBox();
		void 			_receive(Msg* pMsg) override;

		void		_render( GfxDevice * pDevice, const RectSPX& _canvas, const RectSPX& _window ) override;
		void		_resize(const SizeSPX& size, int scale = -1) override;
		void		_setState(State state) override;

		void		_open();
		void		_close();

		class MySideCanvas : public SideCanvas 
		{ 
			friend class SelectBox; 
			MySideCanvas(SideCanvas::Holder* pHolder) : SideCanvas(pHolder) {}
		};
		typedef	StrongPtr<MySideCanvas>	MySideCanvas_p;
		typedef	WeakPtr<MySideCanvas>	MySideCanvas_wp;

		void		_updateListCanvasOpacity();
		void		_requestRenderEntry(SelectBoxEntry* pEntry);
		void		_markEntry(int idx);
		void		_selectEntry(int idx);
		int			_findEntry(const CoordSPX& ofsInListPanel, CoordSPX * pOfsOut = nullptr);

		inline TextLayout * _listTextLayout() { return m_pListTextLayout ? m_pListTextLayout : Base::defaultTextLayout(); }
		inline const TextLayout * _listTextLayout() const { return m_pListTextLayout ? m_pListTextLayout: Base::defaultTextLayout();  }

		//
		
		void		_didAddEntries(SelectBoxEntry * pEntry, int nb) override;
		void		_didMoveEntries(SelectBoxEntry * pFrom, SelectBoxEntry * pTo, int nb) override;
		void		_willEraseEntries(SelectBoxEntry * pEntry, int nb) override;

		Object *	_object() override { return this; }

		//
		
		spx			_sideCanvasMatchingHeight( const SideCanvas * pCanvas, spx width, int scale = -1 ) const override;
		spx			_sideCanvasMatchingWidth( const SideCanvas * pCanvas, spx height, int scale = -1 ) const override;

		SizeSPX		_sideCanvasDefaultSize( const SideCanvas * pCanvas, int scale = -1 ) const  override;
		void		_sideCanvasRender( SideCanvas * pCanvas, GfxDevice * pDevice, const RectSPX& _canvas, const RectSPX& _window ) override;

		void		_sideCanvasRefresh( SideCanvas * pCanvas) override;
		void		_sideCanvasResize( SideCanvas * pCanvas, const SizeSPX& size, int scale = -1 ) override;

		void		_sideCanvasReceive( SideCanvas * pCanvas,  Msg * pMsg ) override;
		bool		_sideCanvasAlphaTest( SideCanvas * pCanvas,  const CoordSPX& ofs ) override;			
	
		void		_sideCanvasSetSkin(SideCanvas * pCanvas, Skin * pSkin) override;
		void		_sideCanvasSetState(SideCanvas * pCanvas, State state) override;

		

	private:

		MySideCanvas_p	m_pListCanvas;
		TextLayout_p	m_pListTextLayout;
		int				m_selectedEntryIndex = -1;
		int				m_markedEntryIndex = -1;		// Marked through mouse hover or keyboard input.

		Skin_p			m_pEntrySkin;
		TextStyle_p		m_pEntryStyle;

		spx				m_entryContentWidth;			// Width of content of an entry in the list.
		SizeSPX			m_entryContentPaddingSize;		// Size of padding of content of an entry in the list.

		SizeSPX			m_defaultSize;
		spx				m_matchingHeight = 0;			// Matching height for current width.

		spx				m_listCanvasMatchingHeight = 0;
		SizeSPX			m_listCanvasDefaultSize;

		bool			m_bOpen = false;
		State			m_closeState;


	};




} // namespace wg
#endif //WG_SELECTBOX_DOT_H
