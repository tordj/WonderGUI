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
	
	class CharSeq;
	class String;
	class CharBuffer;
	
	class ModValue;
	typedef	WgIStrongPtr<ModValue,Value_p>	ModValue_p;
	typedef	WgIWeakPtr<ModValue,Value_wp>	ModValue_wp;
	
	/**
	 * @brief Interface to a value field where the value is modifiable through the api
	 * 
	 * The value in a modifiable value field can be set through the API, but isn't
	 * editable through the UI.
	 * 
	*/
	
	class ModValue : public Value
	{
	public:
		ModValue(ModValueField * pField) : Value(pField) {}
	
		virtual bool				isInstanceOf( const char * pClassName ) const;
		virtual const char *		className( void ) const;
		static const char			CLASSNAME[];
		static ModValue_p		cast( const Interface_p& pInterface );
		inline ModValue_p		ptr() { return ModValue_p(_object(),this); }
	
	
		void						clear();
		bool						set( int64_t value, int scale = 1);
	
	
	/*	For the future...
		inline void					set( float value );
		inline void					set( double value );
	*/
	
		inline int64_t				value() const { return _field()->value(); }
		inline int					scale() const { return _field()->scale(); }
	
		bool						setRange( int64_t min, int64_t max );
		inline int64_t				min() const { return _field()->min(); }
		inline int64_t				max() const { return _field()->max(); }
	
	private:
		inline	ModValueField * 		_field() { return static_cast<ModValueField*>(m_pField); }
		inline	const ModValueField * _field() const { return static_cast<ModValueField*>(m_pField); }
	};
	
	

} // namespace wg
#endif //WG_MODVALUE_DOT_H
