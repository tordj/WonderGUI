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

#ifndef	WG_GIZMO_TEXT_DOT_H
#define	WG_GIZMO_TEXT_DOT_H


#ifndef WG_GIZMO_DOT_H
#	include <wg_gizmo.h>
#endif

#ifndef	WG_INTERFACE_EDITTEXT_DOT_H
#	include <wg_interface_edittext.h>
#endif

#ifndef	WG_TEXT_DOT_H
#	include <wg_text.h>
#endif

#ifndef WG_CURSORINSTANCE_DOT_H
#	include <wg_cursorinstance.h>
#endif

#ifndef	WG_TOOLTIP_DOT_H
#	include <wg_tooltip.h>
#endif


class WgGizmoText:public WgGizmo, public WgInterfaceEditText
{
public:
	WgGizmoText();
	virtual ~WgGizmoText();
	virtual const char * Type() const;
	static const char * GetMyType();


	//____ Methods __________________________________________

	inline void		SetMaxCharacters( int characters ) { m_maxCharacters = characters; }
	inline int		MaxCharacters() { return m_maxCharacters; }

	inline void		SetMaxLines( int nLines ) { m_maxLines = nLines; }
	inline int		MaxLines() { return m_maxLines; }

	Uint32	InsertTextAtCursor( const WgCharSeq& str );
	bool	InsertCharAtCursor( Uint16 c );

	void		goBOL();
	void		goEOL();
	void		goBOF();
	void		goEOF();

	virtual void		SetInputMode(InputMode mode);
	virtual InputMode	GetInputMode() const { return m_inputMode; }

	WgPointerStyle		GetPointerStyle() const;

	virtual bool TempIsInputField() const { return IsEditable(); }
	virtual Wg_Interface_TextHolder* TempGetText() { return this; }


	int		HeightForWidth( int width ) const;
	WgSize	BestSize() const;

protected:

	bool	IsEditable() const { return m_inputMode == Editable; }
	bool	IsSelectable() const { return m_inputMode != Static; }

	void	OnCloneContent( const WgGizmo * _pOrg );
	void	OnRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip, Uint8 _layer );
	void	OnNewSize( const WgSize& size );
	void	OnRefresh();
	void	OnUpdate( const WgUpdateInfo& _updateInfo );
	void	OnAction( WgInput::UserAction action, int button_key, const WgActionDetails& info, const WgInput& inputObj );
	bool	OnAlphaTest( const WgCord& ofs );
	void	OnEnable();
	void	OnDisable();
	void	OnGotInputFocus();
	void	OnLostInputFocus();

private:
	void	TextModified();
	bool	InsertCharAtCursorInternal( Uint16 c );


	WgText				m_text;
	bool				m_bHasFocus;
	int					m_maxCharacters;
	int					m_maxLines;
	InputMode			m_inputMode;
};





#endif // WG_GIZMO_TEXT_DOT_H
