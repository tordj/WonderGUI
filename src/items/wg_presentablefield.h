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

	void				SetStyle( const WgTextStylePtr& pStyle );
	void				ClearStyle();
	WgTextStylePtr		Style() const { return m_pStyle; }

	void				SetPresenter( const WgTextPresenterPtr& pPresenter );
	void				ClearPresenter();
	WgTextPresenterPtr	Presenter() const { return m_pPresenter; }

	void				SetState( WgState state );
	inline WgState		State() const { return m_state; }

	WgSize				PreferredSize() const;	
	int					MatchingWidth( int height ) const;
	int					MatchingHeight( int width ) const;
	inline WgSize		Size() const { return m_size; }

	int					CoordToChar( WgCoord pos ) const;
	WgRect				CharToRect( int charOfs ) const;

	void				OnRefresh();

	WgString			Tooltip() const;

	void				OnNewSize( const WgSize& size );	
	void				OnRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _clip );

	WgRect				RectForRange( int ofs, int length ) const;
	
	WgString			GetString() const;
	
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

struct WgEditableFieldState
{
	bool bCursor;		// Set if cursor should be displayed.
	int selectBegin;	// Selection is between selectBegin and cursorOfs.
	int cursorOfs;		// End of selection and cursor offset (if displaying)
	int wantedOfs;		// Wanted offset in pixels when skipping between lines.		
};


#endif //WG_PRESENTABLEFIELD_DOT_H