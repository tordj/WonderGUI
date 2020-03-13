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

		SizeI		_textSize() const override;
		State		_textState() const override;
		TextStyle * _textStyle() const override;

		const Char *_textBegin() const override;
		int 		_textLength() const override;

		bool		_caretVisible() const override;
		int			_caretOffset() const override;
		std::tuple<int, int>	_selectedText() const override;

		void		_mapperRequestRender() override;
		void		_mapperRequestRender(const RectI& rect) override;
		void		_mapperRequestResize() override;

		int			m_id;
		int			m_height = 0;
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

		bool				isInstanceOf( const char * pClassName ) const override;
		const char *		className( void ) const override;
		static const char	CLASSNAME[];

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

		SizeI		_preferredSize() const override;
		int			_matchingHeight(int width) const override;
		void		_cloneContent( const Widget * _pOrg ) override;
		void		_render( GfxDevice * pDevice, const RectI& _canvas, const RectI& _window ) override;
		void		_resize(const SizeI& size) override;


		class TextAccess : public CTextDisplay { friend class SelectBox; };
		const TextAccess& _text() const { return static_cast<const TextAccess&>(text); }
		TextAccess& _text() { return static_cast<TextAccess&>(text); }

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
		int			_findEntry(const CoordI& ofsInListPanel, CoordI * pOfsOut = 0);

		inline TextMapper * _listTextMapper() { return m_pListTextMapper ? m_pListTextMapper : Base::defaultTextMapper().rawPtr(); }
		inline const TextMapper * _listTextMapper() const { return m_pListTextMapper ? m_pListTextMapper: Base::defaultTextMapper().rawPtr();  }

		//
		
		void		_didAddEntries(SelectBoxEntry * pEntry, int nb) override;
		void		_didMoveEntries(SelectBoxEntry * pFrom, SelectBoxEntry * pTo, int nb) override;
		void		_willEraseEntries(SelectBoxEntry * pSlot, int nb) override;

		//
		
		int			_sideCanvasMatchingHeight( const SideCanvas * pCanvas, int width ) const override;
		int			_sideCanvasMatchingWidth( const SideCanvas * pCanvas, int height ) const override;

		SizeI		_sideCanvasPreferredSize( const SideCanvas * pCanvas ) const  override;
		void		_sideCanvasRender( SideCanvas * pCanvas, GfxDevice * pDevice, const RectI& _canvas, const RectI& _window ) override;

		void		_sideCanvasRefresh( SideCanvas * pCanvas) override;
		void		_sideCanvasResize( SideCanvas * pCanvas, const SizeI& size ) override;

		void		_sideCanvasReceive( SideCanvas * pCanvas,  Msg * pMsg ) override;
		bool		_sideCanvasAlphaTest( SideCanvas * pCanvas,  const CoordI& ofs ) override;			
	
		void		_sideCanvasSetSkin(SideCanvas * pCanvas, Skin * pSkin) override;
		void		_sideCanvasSetState(SideCanvas * pCanvas, State state) override;

		

	private:

		MySideCanvas_p	m_pListCanvas;
		TextMapper_p	m_pListTextMapper;
		int				m_selectedEntryIndex = -1;
		int				m_hoveredEntryIndex = -1;

		Skin_p			m_pEntrySkin;
		TextStyle_p		m_pEntryStyle;

		int				m_entryContentWidth;			// Width of content of an entry in the list, in quarterpixels.
		BorderI			m_entryContentPadding;			// Padding of content of an entry in the list, in quarterpixels.

		SizeI			m_preferredSize;
		int				m_matchingHeight = 0;			// Matching height for current width.

		int				m_listCanvasMatchingHeight = 0;
		SizeI			m_listCanvasPreferredSize;


	};




} // namespace wg
#endif //WG_SELECTBOX_DOT_H
