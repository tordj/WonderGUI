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

#ifndef	WG_TEXT_DOT_H
#	include <wg_text.h>
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

class WgValueEditor : public WgWidget, public Wg_Interface_ValueHolder
{
	public:
		WgValueEditor();
		virtual ~WgValueEditor();

		virtual const char * Type() const;
		static const char * GetClass();
		virtual WgWidget * NewOfMyType() const { return new WgValueEditor(); };


		//____ Methods __________________________________________

		bool	SetMaxInputChars( int max );
		int		MaxInputChars() const { return m_maxInputChars; }
		void	SetTextAlignment( const WgOrigo alignment );
		WgOrigo GetTextAlignment( ) const;
		void	SetTextColor(WgColor color);
		WgColor GetTextColor() const;
		bool	SetTextprop( const WgTextpropPtr& _pProp );
		WgTextpropPtr GetTextprop( ) const;
		void	SetTextManager(WgTextManager * _pManager);
		WgTextManager * GetTextManager() const;
		void	SetFormat( const WgValueFormat& format );
		const WgValueFormat& GetFormat() const { return m_format; }
		void	Clear();									// Sets value to 0 and clears input field.

		bool	SelectAllText();

		WgSize	PreferredSize() const;


		virtual bool		IsInputField() const	{ return true; }

	protected:

		void	_onCloneContent( const WgWidget * _pOrg );
		void	_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip );
		void	_onEvent( const WgEvent::Event * pEvent, WgEventHandler * pHandler );
		void	_onEnable();
		void	_onDisable();
		void	_onGotInputFocus();
		void	_onLostInputFocus();
		void	_onRefresh();

		WgWidget*	_getWidget() { return this; }

	private:
		void	_limitCursor();					///< Make sure cursor or selection is not in prefix or suffix part of text.
		void	_selectAll();					///< Our own select all that doesn't include prefix or suffix.

		void	_valueModified();				///< Called when value has been modified.
		void	_rangeModified();				///< Called when range (and thus fractional value) has been modified.

		bool	_parseValueFromInput( int64_t * wpResult );

		void	_regenText();

		bool				m_bRegenText;
		WgValueFormat		m_format;			///< Value format specified by user
		WgValueFormat		m_useFormat;		///< Value format currently used (affected by user typing in values).
		WgText				m_text;
		int					m_buttonDownOfs;
		bool				m_bSelectAllOnRelease;
		int					m_maxInputChars;
		WgCoord				m_viewOfs;
};



#endif // WG_VALUEEDITOR_DOT_H
