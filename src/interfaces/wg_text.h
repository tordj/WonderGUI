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

#ifndef WG_TEXT_DOT_H
#define WG_TEXT_DOT_H

#ifndef WG_TEXTFIELD_DOT_H
#	include <wg_textfield.h>
#endif

#ifndef WG_INTERFACE_DOT_H
#	include <wg_interface.h>
#endif

#ifndef WG_POINTERS_DOT_H
#	include <wg_pointers.h>
#endif

#ifndef WG_TEXTSTYLE_DOT_H
#	include <wg_textstyle.h>
#endif

namespace wg 
{
	
	class Color;
	class CharSeq;
	class String;
	class CharBuffer;
	
	class Text;
	typedef	StrongInterfacePtr<Text,Interface_p>		Text_p;
	typedef	WeakInterfacePtr<Text,Interface_wp>	Text_wp;
	
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
	
	class Text : public Interface
	{
	public:
		Text( TextField * pField ) : m_pField(pField) {};
	
		virtual bool			isInstanceOf( const char * pClassName ) const;
		virtual const char *	className( void ) const;
		static const char		CLASSNAME[];
		static Text_p		cast( const Interface_p& pInterface );				// Provided just for completeness sake.
		inline Text_p		ptr() { return Text_p(_object(),this); }
	
		inline void				setStyle( const TextStyle_p& pStyle ) { m_pField->setStyle(pStyle); }
		inline void				clearStyle() { m_pField->clearStyle(); }
		inline TextStyle_p	style() const { return m_pField->style(); }
	
		inline void					setPresenter( const TextPresenter_p& pPresenter ) { m_pField->setPresenter(pPresenter); }
		inline void					clearPresenter() { m_pField->clearPresenter(); }
		inline TextPresenter_p	presenter() const { return m_pField->presenter(); }
	
		inline 	String			get() const { return m_pField->getString(); }
	
		inline State			state() const { return m_pField->state(); }
		inline int				length() const { return m_pField->length(); }
		inline bool				isEmpty() const { return m_pField->isEmpty(); }
	
	protected:
		Object *				_object() const;
	
		TextField * 			m_pField;
	};
	
	

} // namespace wg
#endif //WG_TEXT_DOT_H
