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

#ifndef	WG_GIZMO_CHECKBOX_DOT_H
#define	WG_GIZMO_CHECKBOX_DOT_H

#ifndef	WG_GIZMO_DOT_H
#	include <wg_gizmo.h>
#endif

#ifndef WG_INTERFACE_TEXTHOLDER_DOT_H
#	include <wg_interface_textholder.h>
#endif

#ifndef WG_TEXT_DOT_H
#	include <wg_text.h>
#endif

#ifndef	WG_TOOLTIP_DOT_H
#	include <wg_tooltip.h>
#endif

#ifndef WG_BLOCKSET_DOT_H
#	include <wg_blockset.h>
#endif

class	WgSurface;

class	WgGizmoCheckbox : public WgGizmo, public Wg_Interface_TextHolder
{
public:
	WgGizmoCheckbox();
	virtual ~WgGizmoCheckbox();

	virtual const char * Type() const;
	static const char * GetMyType();

	// for stretched backgrounds

	bool			SetSource( const WgBlockSetPtr& _pUnchecked, const WgBlockSetPtr& _pChecked, bool bFixedSizeBox = false );

	WgBlockSetPtr	GetCheckedSource() const { return m_pBlockChecked; }
	WgBlockSetPtr	GetUncheckedSource() const { return m_pBlockUnchecked; }

	void			SetIcon( const WgBlockSetPtr& _pUnchecked, const WgBlockSetPtr& _pChecked, 
							 WgBorders _areaBorders, const WgOrigo& _origo, float _scale = 0.f, bool _bPushText = true );

	WgBlockSetPtr	GetCheckedIcon() const { return m_pIconChecked; }
	WgBlockSetPtr	GetUncheckedIcon() const { return m_pIconUnchecked; }
	float			GetIconScale() const { return m_iconScale; }
	WgOrigo			GetIconOrigo() const { return m_iconOrigo; }
	WgBorders		GetIconBorders() const { return m_iconAreaBorders; }
	bool			IsIconPushingText() const { return m_bIconPushText; }

    bool			SetDisplacement( Sint8 xUnchecked = 0, Sint8 yUnchecked = 0, Sint8 xOver = 0, Sint8 yOver = 0, Sint8 xChecked = 0, Sint8 yChecked = 0 );
	void			GetDisplacement( Sint8& xUp, Sint8& yUp, Sint8& xOver, Sint8& yOver, Sint8& xDown, Sint8& yDown ) const;

	inline bool		IsChecked() { return m_bChecked; };

	inline bool		Check( void ) { return SetState( true ); };
	inline bool		Uncheck( void ) { return SetState( false ); };
	virtual bool	SetState( bool state );
	inline bool		ChangeState( void ) { return SetState( !m_bChecked ); };

	void			SetTextMouseOverOfsX( Uint16 ofs );
	Uint16			GetTextMouseOverOfsX( )	{ return m_textMouseOverOfsX; }

	bool			SetFixedSize(bool bFixedSizeBox);
	bool			GetFixedSize() const { return m_bFixedSizeBox; }

	Uint32			GetTextAreaWidth();

//    bool			SetTextOffset( Sint8 x, Sint8 y = 0, Sint8 x2 = 0, Sint8 y2 = 0 );
	int				GetLineWidth() const;

	DECLARE_TOOLTIP_SUPPORT();

protected:
	WgRect	GetIconRect( const WgSize& gizmoSize );
	WgRect	GetContentRect( const WgSize& gizmoSize, const WgRect& iconRect );



	void	_onCloneContent( const WgGizmo * _pOrg );
	void	_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip, Uint8 _layer );
	void	_onAction( WgInput::UserAction action, int button_key, const WgActionDetails& info, const WgInput& inputObj );
	void	_onRefresh();
	void	_onNewSize( const WgSize& size );
	bool	_onAlphaTest( const WgCord& ofs );
	void	_onEnable();
	void	_onDisable();
	
	bool			m_bChecked;

private:


	void	RefreshTextArea();
	bool	MarkTestTextArea( int _x, int _y );
	
	virtual void TextModified();


	bool			m_bOver;						// Set when mouse is over.							
	bool			m_bPressed;						// Set when mouse is pressed and over.

	WgBlockSetPtr	m_pIconUnchecked;
	WgBlockSetPtr	m_pIconChecked;
	WgOrigo			m_iconOrigo;
	float			m_iconScale;					// Range: 0.f -> 1.f. 0.f = Fixed size.
	bool			m_bIconPushText;
	WgBorders		m_iconAreaBorders;

	WgBlockSetPtr	m_pBlockUnchecked;
	WgBlockSetPtr	m_pBlockChecked;
	bool			m_bFixedSizeBox;				// Set if checkbox block shouldn't be stretched.
													// Checkbox will then be to left of widget and rest of
													// the area only for the text.
	WgText			m_text;

	WgCord8			m_aDisplace[3];					// Text/icon displacement for unchecked, mouse over and checked.
	Uint16			m_textMouseOverOfsX;
	WgRect*			m_pTextArea;					// For marktesting the text area
	Uint16			m_textAreaCount;

};




#endif //	WG_GIZMO_CHECKBOX_DOT_H
