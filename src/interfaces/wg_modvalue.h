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

#ifndef WG_MODVALUE_DOT_H
#define WG_MODVALUE_DOT_H

#ifndef WG_VALUE_DOT_H
#	include <wg_value.h>
#endif

#ifndef WG_MODVALUEFIELD_DOT_H
#	include <wg_modvaluefield.h>
#endif

namespace wg 
{
	
	class WgCharSeq;
	class WgString;
	class WgCharBuffer;
	
	class WgModValue;
	typedef	WgIStrongPtr<WgModValue,WgValue_p>	WgModValue_p;
	typedef	WgIWeakPtr<WgModValue,WgValue_wp>	WgModValue_wp;
	
	/**
	 * @brief Interface to a value field where the value is modifiable through the api
	 * 
	 * The value in a modifiable value field can be set through the API, but isn't
	 * editable through the UI.
	 * 
	*/
	
	class WgModValue : public WgValue
	{
	public:
		WgModValue(WgModValueField * pField) : WgValue(pField) {}
	
		virtual bool				isInstanceOf( const char * pClassName ) const;
		virtual const char *		className( void ) const;
		static const char			CLASSNAME[];
		static WgModValue_p		cast( const WgInterface_p& pInterface );
		inline WgModValue_p		ptr() { return WgModValue_p(_object(),this); }
	
	
		void						clear();
		bool						set( Sint64 value, int scale = 1);
	
	
	/*	For the future...
		inline void					set( float value );
		inline void					set( double value );
	*/
	
		inline Sint64				value() const { return _field()->value(); }
		inline int					scale() const { return _field()->scale(); }
	
		bool						setRange( Sint64 min, Sint64 max );
		inline Sint64				min() const { return _field()->min(); }
		inline Sint64				max() const { return _field()->max(); }
	
	private:
		inline	WgModValueField * 		_field() { return static_cast<WgModValueField*>(m_pField); }
		inline	const WgModValueField * _field() const { return static_cast<WgModValueField*>(m_pField); }
	};
	
	

} // namespace wg
#endif //WG_MODVALUE_DOT_H
