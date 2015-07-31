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

#ifndef	WG_PRESENTABLEFIELD_DOT_H
#define WG_PRESENTABLEFIELD_DOT_H

#ifndef WG_FIELD_DOT_H
#	include <wg_field.h>
#endif

#ifndef WG_TEXTSTYLE_DOT_H
#	include <wg_textstyle.h>
#endif

#ifndef WG_TEXTPRESENTER_DOT_H
#	include <wg_textpresenter.h>
#endif

#ifndef WG_BASE_DOT_H
#	include <wg_base.h>
#endif

namespace wg 
{
	
	
	//____ WgPresentableHolder ___________________________________________________________
	
	struct WgPresentableHolder : public WgFieldHolder
	{
	};
	
	//____ WgPresentableField __________________________________________________________________
	
	class WgPresentableField : public WgField
	{
		friend class WgTextPresenter;
	public:
		WgPresentableField( WgPresentableHolder * pHolder );
		virtual ~WgPresentableField();
	
		virtual void		setStyle( const WgTextStyle_p& pStyle );
		virtual void		clearStyle();
		WgTextStyle_p		style() const { return m_pStyle; }
	
		virtual void		setPresenter( const WgTextPresenter_p& pPresenter );
		virtual void		clearPresenter();
		WgTextPresenter_p	presenter() const { return m_pPresenter; }
	
		virtual void		setState( WgState state );
		inline WgState		state() const { return m_state; }
	
		virtual WgSize		preferredSize() const;	
		virtual int			matchingWidth( int height ) const;
		virtual int			matchingHeight( int width ) const;
		inline WgSize		size() const { return m_size; }
	
		virtual int			coordToChar( WgCoord pos ) const;
		virtual WgRect		charToRect( int charOfs ) const;
	
		virtual void		onRefresh();
	
		virtual WgString	tooltip() const;
	
		virtual void		onNewSize( const WgSize& size );	
		virtual void		onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _clip );
	
		virtual WgRect		rectForRange( int ofs, int length ) const;
		
		virtual WgString	getString() const;
		
		virtual int			selectionBegin() const;
		virtual int			selectionEnd() const;
		
	protected:
	
		WgTextPresenter *	_presenter() const { return m_pPresenter ? m_pPresenter.rawPtr() : WgBase::defaultPresenter().rawPtr(); }
		WgTextStyle *		_style() const { if( m_pStyle ) return m_pStyle.rawPtr(); return WgBase::defaultStyle().rawPtr(); }
	
		WgSize					m_size;
	
		union 
		{
			void *			m_pPresenterData;
			int				m_presenterData;
		};
	
		WgState					m_state;
		WgTextStyle_p			m_pStyle;
		WgCharBuffer			m_charBuffer;
		WgTextPresenter_p		m_pPresenter;
	
	};
	
	struct WgEditState
	{
		bool 			bCaret;			// Set if cursor should be displayed.
		int 			selectOfs;		// Selection is between selectOfs and caretOfs.
		int				caretOfs;		// End of selection and caret offset (if displaying)
		int				wantedOfs;		// Carets wanted offset in pixels when skipping between lines. -1 = none set.		
	};
	
	

} // namespace wg
#endif //WG_PRESENTABLEFIELD_DOT_H
