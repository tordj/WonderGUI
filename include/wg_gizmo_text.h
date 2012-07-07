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

class WgGizmoText:public WgGizmo, public WgInterfaceEditText
{
public:
	WgGizmoText();
	virtual ~WgGizmoText();
	virtual const char * Type() const;
	static const char * GetMyType();
	virtual WgGizmo * NewOfMyType() const { return new WgGizmoText(); };



	//____ Methods __________________________________________

	inline void		SetMaxLines( int nLines ) { m_maxLines = nLines; }
	inline int		MaxLines() { return m_maxLines; }

	Uint32	InsertTextAtCursor( const WgCharSeq& str );
	bool	InsertCharAtCursor( Uint16 c );

	void		goBOL();
	void		goEOL();
	void		goBOF();
	void		goEOF();

	virtual void			SetEditMode(WgTextEditMode mode);
	virtual WgTextEditMode	GetEditMode() const { return m_text.GetEditMode(); }

	WgPointerStyle		GetPointerStyle() const;
	WgString			GetTooltipString() const;

	virtual bool TempIsInputField() const { return IsEditable(); }
	virtual Wg_Interface_TextHolder* TempGetText() { return this; }

	int		HeightForWidth( int width ) const;
	WgSize	DefaultSize() const;
	bool	IsAutoEllipsisDefault() const { return true; };

	bool	IsEditable() const { return m_text.IsEditable(); }
	bool	IsSelectable() const { return m_text.IsSelectable(); }

protected:

	void	_onCloneContent( const WgGizmo * _pOrg );
	void	_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip, Uint8 _layer );
	void	_onNewSize( const WgSize& size );
	void	_onRefresh();
	void	_onEvent( const WgEvent::Event * pEvent, WgEventHandler * pHandler );
	bool	_onAlphaTest( const WgCoord& ofs );
	void	_onEnable();
	void	_onDisable();
	void	_onGotInputFocus();
	void	_onLostInputFocus();

private:
	void	_textModified();
	bool	_insertCharAtCursor( Uint16 c );


	WgText				m_text;
	bool				m_bHasFocus;
	int					m_maxLines;
	bool				m_bResetCursorOnFocus;
};





#endif // WG_GIZMO_TEXT_DOT_H
