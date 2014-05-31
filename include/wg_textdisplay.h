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

#ifndef	WG_TEXTDISPLAY_DOT_H
#define	WG_TEXTDISPLAY_DOT_H


#ifndef WG_WIDGET_DOT_H
#	include <wg_widget.h>
#endif

#ifndef	WG_TEXTFIELD_DOT_H
#	include <wg_textfield.h>
#endif

#ifndef WG_CURSORINSTANCE_DOT_H
#	include <wg_cursorinstance.h>
#endif

class WgTextDisplay;
typedef	WgSmartPtr<WgTextDisplay,WgWidgetPtr>		WgTextDisplayPtr;
typedef	WgWeakPtr<WgTextDisplay,WgWidgetWeakPtr>	WgTextDisplayWeakPtr;

class WgTextDisplay:public WgWidget, public WgTextHolder
{
public:
	static WgTextDisplayPtr	Create() { return WgTextDisplayPtr(new WgTextDisplay()); }

	bool		IsInstanceOf( const char * pClassName ) const;
	const char *ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgTextDisplayPtr	Cast( const WgObjectPtr& pObject );

	//____ Methods __________________________________________

	inline void		SetMaxLines( int nLines ) { m_maxLines = nLines; }
	inline int		MaxLines() { return m_maxLines; }

	int		InsertTextAtCursor( const WgCharSeq& str );
	bool	InsertCharAtCursor( Uint16 c );

	virtual void			SetEditMode(WgTextEditMode mode);
	virtual WgTextEditMode	EditMode() const { return m_text.EditMode(); }

	WgPointerStyle		PointerStyle() const;
	WgString			TooltipString() const;

	inline WgIEditTextPtr	Text() { return WgIEditTextPtr(this,&m_text); } 

	int		HeightForWidth( int width ) const;
	WgSize	PreferredSize() const;
	bool	IsAutoEllipsisDefault() const { return true; };

	bool	IsEditable() const { return m_text.IsEditable(); }
	bool	IsSelectable() const { return m_text.IsSelectable(); }

protected:
	WgTextDisplay();
	virtual ~WgTextDisplay();
	virtual WgWidget* _newOfMyType() const { return new WgTextDisplay(); };

	void	_onCloneContent( const WgWidget * _pOrg );
	void	_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip );
	void	_onNewSize( const WgSize& size );
	void	_onRefresh();
	void	_onEvent( const WgEventPtr& pEvent, WgEventHandler * pHandler );
	void	_onStateChanged( WgState oldState );
	void	_onSkinChanged( const WgSkinPtr& pOldSkin, const WgSkinPtr& pNewSkin );

private:
	void	_fieldModified( WgTextField * pField );
	bool	_insertCharAtCursor( Uint16 c );


	WgTextField			m_text;
	bool				m_bHasFocus;
	int					m_maxLines;
	bool				m_bResetCursorOnFocus;
};





#endif // WG_TEXTDISPLAY_DOT_H
