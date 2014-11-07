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

#ifndef	WG_EDITTEXT_DOT_H
#	include <wg_edittext.h>
#endif

#ifndef WG_CARETINSTANCE_DOT_H
#	include <wg_caretinstance.h>
#endif

class WgLineEditor;
typedef	WgStrongPtr<WgLineEditor,WgWidgetPtr>		WgLineEditorPtr;
typedef	WgWeakPtr<WgLineEditor,WgWidgetWeakPtr>	WgLineEditorWeakPtr;

//____ WgLineEditor ____________________________________________________________

class WgLineEditor : public WgWidget, protected WgLegacyTextHolder
{
public:
	static WgLineEditorPtr	Create() { return WgLineEditorPtr(new WgLineEditor()); }

	bool		IsInstanceOf( const char * pClassName ) const;
	const char *ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgLineEditorPtr	Cast( const WgObjectPtr& pObject );

	//____ Interfaces ______________________________________

	WgEditText		text;

	//____ Methods __________________________________________

	inline void	SetPasswordMode( bool on_off ) { m_bPasswordMode = on_off; };
	inline bool PasswordMode() {return m_bPasswordMode;};
	inline Uint16 PasswordGlyph() const				 { return m_pwGlyph; };
	void		SetPasswordGlyph( Uint16 glyph );

	int			InsertTextAtCursor( const WgCharSeq& str );
	bool		InsertCharAtCursor( Uint16 c );

	virtual void			SetEditMode(WgTextEditMode mode);
	virtual WgTextEditMode	EditMode() const { return m_text.EditMode(); }

	WgSize		PreferredSize() const;
	bool		IsAutoEllipsisDefault() const { return false; };


protected:
	WgLineEditor();
	virtual ~WgLineEditor();
	virtual WgWidget* _newOfMyType() const { return new WgLineEditor(); };

	bool	_isEditable() const { return m_text.IsEditable(); }
	bool	_isSelectable() const { return m_text.IsSelectable(); }

	void	_onEvent( const WgEventPtr& pEvent, WgEventHandler * pHandler );
	void	_onStateChanged( WgState oldState );
	void	_onCloneContent( const WgWidget * _pOrg );
	void	_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip );
	void	_onNewSize( const WgSize& size );
	void	_onSkinChanged( const WgSkinPtr& pOldSkin, const WgSkinPtr& pNewSkin );

	WgObject * 		_object() { return this; };
	void			_onFieldDirty( WgField * pField );
	void 			_onFieldResize( WgField * pField );

private:

	void	_adjustViewOfs();

	WgLegacyTextField			m_text;

	bool				m_bResetCursorOnFocus;
	bool				m_bPasswordMode;
	Uint16				m_pwGlyph;
	int					m_viewOfs;
};


#endif //WG_LINEEDITOR_DOT_H
