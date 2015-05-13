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

	virtual void		SetStyle( const WgTextStylePtr& pStyle );
	virtual void		ClearStyle();
	WgTextStylePtr		Style() const { return m_pStyle; }

	virtual void		SetPresenter( const WgTextPresenterPtr& pPresenter );
	virtual void		ClearPresenter();
	WgTextPresenterPtr	Presenter() const { return m_pPresenter; }

	virtual void		SetState( WgState state );
	inline WgState		State() const { return m_state; }

	virtual WgSize		PreferredSize() const;	
	virtual int			MatchingWidth( int height ) const;
	virtual int			MatchingHeight( int width ) const;
	inline WgSize		Size() const { return m_size; }

	virtual int			CoordToChar( WgCoord pos ) const;
	virtual WgRect		CharToRect( int charOfs ) const;

	virtual void		OnRefresh();

	virtual WgString	Tooltip() const;

	virtual void		OnNewSize( const WgSize& size );	
	virtual void		OnRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _clip );

	virtual WgRect		RectForRange( int ofs, int length ) const;
	
	virtual WgString	GetString() const;
	
	virtual int			SelectionBegin() const;
	virtual int			SelectionEnd() const;
	
protected:

	WgTextPresenter *	_presenter() const { return m_pPresenter ? m_pPresenter.RawPtr() : WgBase::DefaultPresenter().RawPtr(); }
	WgTextStyle *		_style() const { if( m_pStyle ) return m_pStyle.RawPtr(); return WgBase::DefaultStyle().RawPtr(); }

	WgSize					m_size;

	union 
	{
		void *			m_pPresenterData;
		int				m_presenterData;
	};

	WgState					m_state;
	WgTextStylePtr			m_pStyle;
	WgCharBuffer			m_charBuffer;
	WgTextPresenterPtr		m_pPresenter;

};

struct WgEditState
{
	bool 			bCaret;			// Set if cursor should be displayed.
	int 			selectOfs;		// Selection is between selectOfs and caretOfs.
	int				caretOfs;		// End of selection and caret offset (if displaying)
	int				wantedOfs;		// Carets wanted offset in pixels when skipping between lines. -1 = none set.		
};


#endif //WG_PRESENTABLEFIELD_DOT_H