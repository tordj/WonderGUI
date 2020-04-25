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
#include <wg_cvector.h>

namespace wg
{

	class SelectBox;
	
	//____ SelectBoxEntry ________________________________________________________

	class SelectBoxEntry : private Text
	{
		friend class SelectBox;
	public:
		SelectBoxEntry( int id, String& text ) : m_id(id), m_string(text) {};
		
		inline int		id() const { return m_id; }
		inline String	text() const { return m_string; }

	protected:

		// Overloaded from Text

		Size		_textSize() const override;
		State		_textState() const override;
		TextStyle * _textStyle() const override;

		const Char *_textBegin() const override;
		int 		_textLength() const override;

		bool		_caretVisible() const override;
		int			_caretOffset() const override;
		std::tuple<int, int>	_selectedText() const override;

		void		_mapperRequestRender() override;
		void		_mapperRequestRender(const Rect& rect) override;
		void		_mapperRequestResize() override;

		int			m_id;
		MU			m_height;
		String 		m_string;
		SelectBox * m_pParent = nullptr;
	};
		

	//____ SelectBox __________________________________________________________

	class SelectBox;
	typedef	StrongPtr<SelectBox>		SelectBox_p;
	typedef	WeakPtr<SelectBox>		SelectBox_wp;


	class SelectBox : public Widget, protected CVector<SelectBoxEntry>::Holder, protected SideCanvas::Holder
	{
		friend class SelectBoxEntry;
	public:
		//.____ Creation __________________________________________

		static SelectBox_p	create() { return SelectBox_p(new SelectBox()); }

		//.____ Components ____________________________________

		CTextDisplay			text;
		CVector<SelectBoxEntry>	entries;

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Geometry ______________________________________________________

		Size		preferredSize() const override;
		MU			matchingHeight(MU width) const override;

		//.____ Appearance ________________________________________________________

		void			setEntrySkin(Skin * pSkin);
		Skin_p			entrySkin() const { return m_pEntrySkin;  }

		void			setEntryStyle(TextStyle * pStyle);
		TextStyle_p		entryStyle() const { return m_pEntryStyle; }

		void			setListTextMapper(TextMapper * pTextMapper);
		TextMapper_p	listTextMapper() const { return m_pListTextMapper; }

		void			setListSkin(Skin * pSkin);
		Skin_p			listSkin() const { return m_pListCanvas->skin(); }

		//.___ Control _____________________________________________________________

		void			selectEntry(const CVector<SelectBoxEntry>::iterator& it);
		bool			selectEntryById(int id);
		void			selectEntryByIndex(int index);

		int				selectedEntryId() const;
		inline int		selectedEntryIndex() const { return m_selectedEntryIndex; }

		//.____ Misc _________________________________________________________________

		void 			receive(Msg * pMsg) override;


	protected:
		SelectBox();
		virtual ~SelectBox();
		virtual Widget* _newOfMyType() const override { return new SelectBox(); };

		void		_cloneContent( const Widget * _pOrg ) override;
		void		_render( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window ) override;
		void		_resize(const Size& size) override;


		class MySideCanvas : public SideCanvas 
		{ 
			friend class SelectBox; 
			MySideCanvas(SideCanvas::Holder* pHolder) : SideCanvas(pHolder) {}
		};
		typedef	StrongPtr<MySideCanvas>	MySideCanvas_p;
		typedef	WeakPtr<MySideCanvas>	MySideCanvas_wp;

		void		_updateListCanvasOpacity();
		void		_requestRenderEntry(SelectBoxEntry* pEntry);
		void		_selectEntry(int idx);
		State		_entryState(int idx);
		int			_findEntry(const Coord& ofsInListPanel, Coord * pOfsOut = 0);

		inline TextMapper * _listTextMapper() { return m_pListTextMapper ? m_pListTextMapper : Base::defaultTextMapper(); }
		inline const TextMapper * _listTextMapper() const { return m_pListTextMapper ? m_pListTextMapper: Base::defaultTextMapper();  }

		//
		
		void		_didAddEntries(SelectBoxEntry * pEntry, int nb) override;
		void		_didMoveEntries(SelectBoxEntry * pFrom, SelectBoxEntry * pTo, int nb) override;
		void		_willEraseEntries(SelectBoxEntry * pSlot, int nb) override;

		Object *	_object() override { return this; }

		//
		
		MU			_sideCanvasMatchingHeight( const SideCanvas * pCanvas, MU width ) const override;
		MU			_sideCanvasMatchingWidth( const SideCanvas * pCanvas, MU height ) const override;

		Size		_sideCanvasPreferredSize( const SideCanvas * pCanvas ) const  override;
		void		_sideCanvasRender( SideCanvas * pCanvas, GfxDevice * pDevice, const Rect& _canvas, const Rect& _window ) override;

		void		_sideCanvasRefresh( SideCanvas * pCanvas) override;
		void		_sideCanvasResize( SideCanvas * pCanvas, const Size& size ) override;

		void		_sideCanvasReceive( SideCanvas * pCanvas,  Msg * pMsg ) override;
		bool		_sideCanvasAlphaTest( SideCanvas * pCanvas,  const Coord& ofs ) override;			
	
		void		_sideCanvasSetSkin(SideCanvas * pCanvas, Skin * pSkin) override;
		void		_sideCanvasSetState(SideCanvas * pCanvas, State state) override;

		

	private:

		MySideCanvas_p	m_pListCanvas;
		TextMapper_p	m_pListTextMapper;
		int				m_selectedEntryIndex = -1;
		int				m_hoveredEntryIndex = -1;

		Skin_p			m_pEntrySkin;
		TextStyle_p		m_pEntryStyle;

		MU				m_entryContentWidth;			// Width of content of an entry in the list, in quarterpixels.
		Border			m_entryContentPadding;			// Padding of content of an entry in the list, in quarterpixels.

		Size			m_preferredSize;
		MU				m_matchingHeight = 0;			// Matching height for current width.

		MU				m_listCanvasMatchingHeight = 0;
		Size			m_listCanvasPreferredSize;


	};




} // namespace wg
#endif //WG_SELECTBOX_DOT_H
