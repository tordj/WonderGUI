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
#ifndef WG_SWITCH_DOT_H
#define WG_SWITCH_DOT_H


#ifndef WG_WIDGET_DOT_H
#	include <wg_widget.h>
#endif

namespace wg 
{
	
	class Switch;
	typedef	StrongPtr<Switch,Widget_p>		Switch_p;
	typedef	WeakPtr<Switch,Widget_wp>	Switch_wp;
	
	//____ Switch ____________________________________________________________
	
	class Switch : public Widget
	{
	public:
		static Switch_p		create() { return Switch_p(new Switch()); }
	
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static Switch_p		cast( const Object_p& pObject );
	
	    void    setValue( int value );
	    
		Size	preferredSize() const;
	
	protected:
		Switch();
		virtual ~Switch();
		virtual Widget* _newOfMyType() const { return new Switch(); };
	
		void	_cloneContent( const Widget * _pOrg );
		void	_render( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, const Rect& _clip );
		bool	_alphaTest( const Coord& ofs );
	
	private:
	
	};
	
	

} // namespace wg
#endif //WG_SWITCH_DOT_H
