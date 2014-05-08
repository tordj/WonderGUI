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

#ifndef	WG_VALUEEDITOR_DOT_H
#define	WG_VALUEEDITOR_DOT_H

#ifndef	WG_WIDGET_DOT_H
#	include <wg_widget.h>
#endif

#ifndef	WG_TEXTFIELD_DOT_H
#	include <wg_textfield.h>
#endif

#ifndef WG_VALUEFORMAT_DOT_H
#	include <wg_valueformat.h>
#endif

#ifndef WG_INTERFACE_VALUEHOLDER_DOT_H
#	include <wg_interface_valueholder.h>
#endif


class	WgCursorInstance;
class	WgFont;
class	WgTextManager;

class WgValueEditor;
typedef	WgSmartPtr<WgValueEditor,WgWidgetPtr>		WgValueEditorPtr;
typedef	WgWeakPtr<WgValueEditor,WgWidgetWeakPtr>	WgValueEditorWeakPtr;

class WgValueEditor : public WgWidget, public Wg_Interface_ValueHolder, private WgTextHolder
{
public:
	static WgValueEditorPtr	Create() { return WgValueEditorPtr(new WgValueEditor()); }

	bool		IsInstanceOf( const char * pClassName ) const;
	const char *ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgValueEditorPtr	Cast( const WgObjectPtr& pObject );


	//____ Methods __________________________________________

	bool	SetMaxInputChars( int max );
	int		MaxInputChars() const { return m_maxInputChars; }
	void	SetFormat( const WgValueFormatPtr& pFormat );
	WgValueFormatPtr Format() const { return m_pFormat; }
	void	Clear();									// Sets value to 0 and clears input field.

	inline WgIStaticTextPtr	Text() { return WgIStaticTextPtr(this,&m_text); } 
	WgSize	PreferredSize() const;

protected:
	WgValueEditor();
	virtual ~WgValueEditor();
	virtual WgWidget* _newOfMyType() const { return new WgValueEditor(); };

	void	_onCloneContent( const WgWidget * _pOrg );
	void	_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip );
	void	_onEvent( const WgEventPtr& pEvent, WgEventHandler * pHandler );
	void	_onStateChanged( WgState oldState, WgState newState );
	void	_onSkinChanged( const WgSkinPtr& pOldSkin, const WgSkinPtr& pNewSkin );
	void	_onRefresh();

	WgWidget*	_getWidget() { return this; }

private:
	void	_limitCursor();					///< Make sure cursor or selection is not in prefix or suffix part of text.
	void	_selectAll();					///< Our own select all that doesn't include prefix or suffix.

	void	_valueModified();				///< Called when value has been modified.
	void	_rangeModified();				///< Called when range (and thus fractional value) has been modified.
	void	_textModified( WgTextField * pText );

	bool	_parseValueFromInput( int64_t * wpResult );

	void	_regenText();

	bool				m_bRegenText;
	WgValueFormatPtr	m_pFormat;			///< Value format specified by user
	WgValueFormatPtr	m_pUseFormat;		///< Value format currently used (affected by user typing in values).
	WgTextField			m_text;
	int					m_buttonDownOfs;
	bool				m_bSelectAllOnRelease;
	int					m_maxInputChars;
	WgCoord				m_viewOfs;
};



#endif // WG_VALUEEDITOR_DOT_H
