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

namespace wg 
{
	
	
	class	CaretInstance;
	class	Font;
	class	TextManager;
	
	class ValueEditor;
	typedef	StrongPtr<ValueEditor,Widget_p>		ValueEditor_p;
	typedef	WeakPtr<ValueEditor,Widget_wp>	ValueEditor_wp;
	
	class ValueEditor : public Widget, public _Interface_ValueHolder, protected LegacyTextHolder
	{
	public:
		static ValueEditor_p	create() { return ValueEditor_p(new ValueEditor()); }
	
		bool		isInstanceOf( const char * pClassName ) const;
		const char *className( void ) const;
		static const char	CLASSNAME[];
		static ValueEditor_p	cast( const Object_p& pObject );
	
		//____ Interfaces ______________________________________
	
		LegacyText	text;
	
	
		//____ Methods __________________________________________
	
		bool	setMaxInputChars( int max );
		int		maxInputChars() const { return m_maxInputChars; }
		void	setFormat( const ValueFormat_p& pFormat );
		ValueFormat_p format() const { return m_pFormat; }
		void	clear();									// Sets value to 0 and clears input field.
	
		Size	preferredSize() const;
	
	protected:
		ValueEditor();
		virtual ~ValueEditor();
		virtual Widget* _newOfMyType() const { return new ValueEditor(); };
	
		void	_onCloneContent( const Widget * _pOrg );
		void	_onRender( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, const Rect& _clip );
		void	_onMsg( const Msg_p& pMsg );
		void	_onStateChanged( State oldState );
		void	_onSkinChanged( const Skin_p& pOldSkin, const Skin_p& pNewSkin );
		void	_onRefresh();
	
		Widget*	_getWidget() { return this; }
	
	private:
		void	_limitCursor();					///< Make sure cursor or selection is not in prefix or suffix part of text.
		void	_selectAll();					///< Our own select all that doesn't include prefix or suffix.
	
		void	_valueModified();				///< Called when value has been modified.
		void	_rangeModified();				///< Called when range (and thus fractional value) has been modified.
	
		Object * _object() { return this; }
		void	_onFieldDirty( Field * pField );
		void	_onFieldResize( Field * pField );
	
		bool	_parseValueFromInput( int64_t * wpResult );
	
		void	_regenText();
	
		bool				m_bRegenText;
		ValueFormat_p	m_pFormat;			///< Value format specified by user
		ValueFormat_p	m_pUseFormat;		///< Value format currently used (affected by user typing in values).
		LegacyTextField			m_text;
		int					m_buttonDownOfs;
		bool				m_bSelectAllOnRelease;
		int					m_maxInputChars;
		Coord				m_viewOfs;
		RouteId			m_tickRouteId;
	};
	
	
	

} // namespace wg
#endif // WG_VALUEEDITOR_DOT_H
