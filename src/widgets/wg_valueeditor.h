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

#ifndef	WG_LEGACYTEXT_DOT_H
#	include <wg_legacytext.h>
#endif

#ifndef WG_VALUEFORMAT_DOT_H
#	include <wg_valueformat.h>
#endif

#ifndef WG_INTERFACE_VALUEHOLDER_DOT_H
#	include <wg_interface_valueholder.h>
#endif


class	WgCaretInstance;
class	WgFont;
class	WgTextManager;

class WgValueEditor;
typedef	WgStrongPtr<WgValueEditor,WgWidget_p>		WgValueEditor_p;
typedef	WgWeakPtr<WgValueEditor,WgWidget_wp>	WgValueEditor_wp;

class WgValueEditor : public WgWidget, public Wg_Interface_ValueHolder, protected WgLegacyTextHolder
{
public:
	static WgValueEditor_p	create() { return WgValueEditor_p(new WgValueEditor()); }

	bool		isInstanceOf( const char * pClassName ) const;
	const char *className( void ) const;
	static const char	CLASSNAME[];
	static WgValueEditor_p	cast( const WgObject_p& pObject );

	//____ Interfaces ______________________________________

	WgLegacyText	text;


	//____ Methods __________________________________________

	bool	SetMaxInputChars( int max );
	int		MaxInputChars() const { return m_maxInputChars; }
	void	setFormat( const WgValueFormat_p& pFormat );
	WgValueFormat_p format() const { return m_pFormat; }
	void	clear();									// Sets value to 0 and clears input field.

	WgSize	preferredSize() const;

protected:
	WgValueEditor();
	virtual ~WgValueEditor();
	virtual WgWidget* _newOfMyType() const { return new WgValueEditor(); };

	void	_onCloneContent( const WgWidget * _pOrg );
	void	_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip );
	void	_onMsg( const WgMsg_p& pMsg );
	void	_onStateChanged( WgState oldState );
	void	_onSkinChanged( const WgSkin_p& pOldSkin, const WgSkin_p& pNewSkin );
	void	_onRefresh();

	WgWidget*	_getWidget() { return this; }

private:
	void	_limitCursor();					///< Make sure cursor or selection is not in prefix or suffix part of text.
	void	_selectAll();					///< Our own select all that doesn't include prefix or suffix.

	void	_valueModified();				///< Called when value has been modified.
	void	_rangeModified();				///< Called when range (and thus fractional value) has been modified.

	WgObject * _object() { return this; }
	void	_onFieldDirty( WgField * pField );
	void	_onFieldResize( WgField * pField );

	bool	_parseValueFromInput( int64_t * wpResult );

	void	_regenText();

	bool				m_bRegenText;
	WgValueFormat_p	m_pFormat;			///< Value format specified by user
	WgValueFormat_p	m_pUseFormat;		///< Value format currently used (affected by user typing in values).
	WgLegacyTextField			m_text;
	int					m_buttonDownOfs;
	bool				m_bSelectAllOnRelease;
	int					m_maxInputChars;
	WgCoord				m_viewOfs;
	WgRouteId			m_tickRouteId;
};



#endif // WG_VALUEEDITOR_DOT_H
