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
#ifndef WG_LINEEDITOR_DOT_H
#define WG_LINEEDITOR_DOT_H


#ifndef WG_WIDGET_DOT_H
#	include <wg_widget.h>
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


//____ WgLineEditor ____________________________________________________________

class WgLineEditor : public WgWidget, public WgInterfaceEditText
{
public:
	WgLineEditor();
	virtual ~WgLineEditor();

	virtual const char *Type( void ) const;
	static const char * GetClass();
	virtual WgWidget * NewOfMyType() const { return new WgLineEditor(); };


	//____ Methods __________________________________________

	inline void	SetPasswordMode( bool on_off ) { m_bPasswordMode = on_off; };
	inline bool PasswordMode() {return m_bPasswordMode;};
	inline Uint16 PasswordGlyph() const				 { return m_pwGlyph; };
	void		SetPasswordGlyph( Uint16 glyph );

	void		GoBOL();
	void		GoEOL();
	void		GoBOF() { GoBOL(); }
	void		GoEOF() { GoEOL(); }

	int			InsertTextAtCursor( const WgCharSeq& str );
	bool		InsertCharAtCursor( Uint16 c );

	virtual void			SetEditMode(WgTextEditMode mode);
	virtual WgTextEditMode	EditMode() const { return m_text.EditMode(); }
	virtual bool			IsInputField() const	{ return _isEditable(); }
	virtual Wg_Interface_TextHolder* TextInterface(){ return this; }

	bool		SetTextWrap(bool bWrap);						// Overloading SetTextWrap since we don't support wrapping text.

	WgSize		PreferredSize() const;
	bool		IsAutoEllipsisDefault() const { return false; };


protected:

	bool	_isEditable() const { return m_text.IsEditable(); }
	bool	_isSelectable() const { return m_text.IsSelectable(); }

	void	_onEvent( const WgEvent::Event * pEvent, WgEventHandler * pHandler );
	void	_onStateChanged( WgState oldState, WgState newState );
	void	_onCloneContent( const WgWidget * _pOrg );
	void	_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip );
	void	_onNewSize( const WgSize& size );
	void	_onSkinChanged( const WgSkinPtr& pOldSkin, const WgSkinPtr& pNewSkin );

	void	_textModified();


private:

	void	_adjustViewOfs();

	WgText				m_text;

	bool				m_bResetCursorOnFocus;
	bool				m_bPasswordMode;
	Uint16				m_pwGlyph;
	int					m_viewOfs;
};


#endif //WG_LINEEDITOR_DOT_H
