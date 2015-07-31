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

#ifndef WG_LEGACYTEXT_DOT_H
#define WG_LEGACYTEXT_DOT_H

#ifndef WG_LEGACYTEXTFIELD_DOT_H
#	include <wg_legacytextfield.h>
#endif

#ifndef WG_INTERFACE_DOT_H
#	include <wg_interface.h>
#endif

#ifndef WG_POINTERS_DOT_H
#	include <wg_pointers.h>
#endif

#ifndef WG_CARET_DOT_H
#	include <wg_caret.h>
#endif

#ifndef WG_TEXTLINK_DOT_H
#	include <wg_textlink.h>
#endif

#ifndef WG_FONT_DOT_H
#	include <wg_font.h>
#endif

#ifndef WG_TEXTPROP_DOT_H
#	include <wg_textprop.h>
#endif

namespace wg 
{
	
	class Color;
	class CharSeq;
	class String;
	class CharBuffer;
	
	class LegacyText;
	typedef	WgIStrongPtr<LegacyText,Interface_p>		LegacyText_p;
	typedef	WgIWeakPtr<LegacyText,Interface_wp>	LegacyText_wp;
	
	/**
	 * @brief Interface to a basic text field.
	 *
	 * Interface to a basic text field.
	 *
	 * The text in a basic text field is set by the widget itself and can
	 * not be modified directly either through the API or UI. Only the appearance
	 * of the text can be modified through this interface.
	 * 
	 */
	
	class LegacyText : public Interface
	{
	public:
		LegacyText( LegacyTextField * pField ) : m_pField(pField) {};
	
		virtual bool			isInstanceOf( const char * pClassName ) const;
		virtual const char *	className( void ) const;
		static const char		CLASSNAME[];
		static LegacyText_p		cast( const Interface_p& pInterface );				// Provided just for completeness sake.
		inline LegacyText_p		ptr() { return LegacyText_p(_object(),this); }
	
		inline void				setProperties( const Textprop_p& pProp ) { m_pField->setProperties(pProp); }
		inline void				clearProperties() { m_pField->clearProperties(); }
		inline Textprop_p	properties() const { return m_pField->properties(); }
	
		inline void				setSelectionProperties( const Textprop_p& pProp ) { m_pField->setSelectionProperties(pProp); }
		inline void				clearSelectionProperties() { m_pField->clearSelectionProperties(); }
		inline Textprop_p	selectionProperties() const { return m_pField->selectionProperties(); }
	
		inline void				setLinkProperties( const Textprop_p& pProp ) { m_pField->setLinkProperties(pProp); }
		inline void				clearLinkProperties() { m_pField->clearLinkProperties(); }
		inline Textprop_p	linkProperties() const { return m_pField->linkProperties(); }
	
		inline void				setAlignment( WgOrigo alignment ) { m_pField->setAlignment(alignment); }
		inline WgOrigo			alignment() const { return m_pField->alignment(); }
	
		inline void				setTintMode( WgTintMode mode ) { m_pField->setTintMode(mode); }
		inline WgTintMode		tintMode() const { return m_pField->tintMode(); }
	
		inline void				setLineSpacing( float adjustment ) { m_pField->setLineSpacing(adjustment); }
		inline float			lineSpacing() const { return m_pField->lineSpacing(); }
	
		inline void				setWrap(bool bWrap) { m_pField->setWrap(bWrap); }
		inline bool				wrap() const { return m_pField->wrap(); }
	
		inline void				setAutoEllipsis(bool bAutoEllipsis) { m_pField->setAutoEllipsis(bAutoEllipsis); }
		inline bool				autoEllipsis() const { return m_pField->autoEllipsis(); }
	
		inline State			state() const { return m_pField->state(); }
		inline int				lines() const { return m_pField->lines(); }
		inline int				length() const { return m_pField->length(); }
		inline bool				isEmpty() const { return m_pField->isEmpty(); }
	
	protected:
		Object *				_object() const;
	
		LegacyTextField * 			m_pField;
	};
	
	

} // namespace wg
#endif //WG_LEGACYTEXT_DOT_H
