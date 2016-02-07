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

#ifndef	WG_VALUEDISPLAY_DOT_H
#define	WG_VALUEDISPLAY_DOT_H

#ifndef	WG_WIDGET_DOT_H
#	include <wg_widget.h>
#endif

#ifndef WG_MODVALUE_DOT_H
#	include <wg_modvalue.h>
#endif

namespace wg 
{
	
	
	class ValueDisplay;
	typedef	StrongPtr<ValueDisplay,Widget_p>		ValueDisplay_p;
	typedef	WeakPtr<ValueDisplay,Widget_wp>	ValueDisplay_wp;
	
	class ValueDisplay : public Widget, protected ModValueHolder
	{
	public:
		static ValueDisplay_p	create() { return ValueDisplay_p(new ValueDisplay()); }
	
		bool		isInstanceOf( const char * pClassName ) const;
		const char *className( void ) const;
		static const char	CLASSNAME[];
		static ValueDisplay_p	cast( const Object_p& pObject );
	
		//____ Interfaces _______________________________________
	
		ModValue		value;
	
		//____ Methods __________________________________________
	
		Size	preferredSize() const;
	
	protected:
		ValueDisplay();
		virtual ~ValueDisplay();
		virtual Widget* _newOfMyType() const { return new ValueDisplay(); };
	
		void	_onRefresh();
		void	_onCloneContent( const Widget * _pOrg );
		void	_onRender( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, const Rect& _clip );
		void	_onStateChanged( State oldState );
		void	_onSkinChanged( const Skin_p& pOldSkin, const Skin_p& pNewSkin );
	
	private:
		Object * _object() { return this; }
		void	_onFieldDirty( Field * pField );
		void	_onFieldResize( Field * pField );
		void	_onValueModified( ModValueField * pField );
	
		ModValueField		m_field;
	};
	
	

} // namespace wg
#endif // WG_VALUEDISPLAY_DOT_H
