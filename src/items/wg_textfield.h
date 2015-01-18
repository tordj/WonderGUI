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

#ifndef	WG_TEXTFIELD_DOT_H
#define WG_TEXTFIELD_DOT_H

#ifndef	WG_TYPES_DOT_H
#	include <wg_types.h>
#endif

#ifndef WG_FIELD_DOT_H
#	include <wg_field.h>
#endif

#ifndef WG_TEXTSTYLE_DOT_H
#	include <wg_textstyle.h>
#endif

#ifndef WG_TEXTPRESENTER_DOT_H
#	include <wg_textpresenter.h>
#endif

#ifndef WG_EVEN_DOT_H
#	include <wg_event.h>
#endif

#ifndef WG_BASE_DOT_H
#	include <wg_base.h>
#endif

class WgString;
class WgCharSeq;
class WgCharBuffer;

//____ WgTextHolder ___________________________________________________________

struct WgTextHolder : public WgFieldHolder
{
};

//____ WgTextField __________________________________________________________________

class WgTextField : public WgField
{
	friend class WgTextPresenter;
public:
	WgTextField( WgTextHolder * pHolder );
	~WgTextField();

	void				SetStyle( const WgTextStylePtr& pStyle );
	void				ClearStyle();
	WgTextStylePtr		Style() const { return m_pStyle; }

	void				SetPresenter( const WgTextPresenterPtr& pPresenter );
	void				ClearPresenter();
	WgTextPresenterPtr	Presenter() const { return m_pPresenter; }


	void				Clear();

	void				Set( const WgCharSeq& seq );
	void				Set( const WgCharBuffer * buffer );
	void				Set( const WgString& str );

	int					Append( const WgCharSeq& seq );
	int					Insert( int ofs, const WgCharSeq& seq );
	int					Replace( int ofs, int nDelete, const WgCharSeq& seq );
	int					Delete( int ofs, int len );


	void				SetState( WgState state );
	inline WgState		State() const { return m_state; }

	inline int			Length() const { return m_text.Length(); }
	inline bool			IsEmpty() const { return Length()==0?true:false; }

	WgSize				PreferredSize() const;	
	int					MatchingWidth( int height ) const;
	int					MatchingHeight( int width ) const;
	inline WgSize		Size() const { return m_size; }

	int					CoordToChar( WgCoord pos ) const;
	WgRect				CharToRect( int charOfs ) const;

	
	void				OnRefresh();
	void				OnNewSize( const WgSize& size );	
	bool 				OnEvent( const WgEventPtr& pEvent, WgEventHandler * pEventHandler, const WgRect& container );
	void				OnRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _clip );

	WgString			Get() const;

	WgRect				RectForRange( int ofs, int length ) const;


protected:

	WgTextPresenter *	_presenter() const { if( m_pPresenter ) return m_pPresenter.RawPtr(); return WgBase::DefaultPresenter().RawPtr(); }
	WgTextStyle *		_style() const { if( m_pStyle ) return m_pStyle.RawPtr(); return WgBase::DefaultStyle().RawPtr(); }

	WgCharBuffer		m_text;
	WgTextStylePtr		m_pStyle;
	WgTextPresenterPtr	m_pPresenter;
	union 
	{
		void *			m_pPresenterData;
		int				m_presenterData;
	};
	WgSize				m_size;
	WgState				m_state;
};

#endif //WG_TEXTFIELD_DOT_H
