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


//____ WgTextHolder ___________________________________________________________

struct WgTextHolder : public WgFieldHolder
{
};

//____ WgTextField __________________________________________________________________

class WgTextField : public WgField
{
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

	int					Length() const;
	inline bool			IsEmpty() const { return Length()==0?true:false; }

	WgSize				PreferredSize() const;	
	int					MatchingWidth( int height ) const;
	int					MatchingHeight( int width ) const;
	inline WgSize		Size() const { return m_size; }

	int					CoordToChar( WgCoord pos );
	WgRect				CharToRect( int charOfs ) const;

	
	void				OnRefresh();
	void				OnNewSize( const WgSize& size );	
	bool 				OnEvent( const WgEventPtr& pEvent, WgEventHandler * pEventHandler, const WgRect& container );
	void				OnRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _clip );

	WgString			Get() const;

	WgRect				RectForRange( int ofs, int length ) const;


protected:
	WgCharBuffer		m_text;
	WgTextStylePtr		m_pStyle;
	WgTextPresenterPtr	m_pPresenter;
	void *				m_pPresenterData;
	WgSize				m_size;
	WgState				m_state;
};

#endif //WG_TEXTFIELD_DOT_H
