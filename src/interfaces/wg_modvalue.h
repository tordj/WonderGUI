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

class WgCharSeq;
class WgString;
class WgCharBuffer;

class WgModValue;
typedef	WgIStrongPtr<WgModValue,WgValuePtr>	WgModValuePtr;
typedef	WgIWeakPtr<WgModValue,WgValueWeakPtr>	WgModValueWeakPtr;

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

	virtual bool				IsInstanceOf( const char * pClassName ) const;
	virtual const char *		ClassName( void ) const;
	static const char			CLASSNAME[];
	static WgModValuePtr		Cast( const WgInterfacePtr& pInterface );
	inline WgModValuePtr		Ptr() { return WgModValuePtr(_object(),this); }


	inline void					Clear() { _field()->Clear(); }
	inline bool					Set( Sint64 value, int scale = 1) { return _field()->Set(value,scale); }


/*	For the future...
	inline void					Set( float value );
	inline void					Set( double value );
*/

	inline Sint64				Value() const { return _field()->Value(); }
	inline int					Scale() const { return _field()->Scale(); }

	inline bool					SetRange( Sint64 min, Sint64 max ) { return _field()->SetRange(min,max); }
	inline Sint64				Min() const { return _field()->Min(); }
	inline Sint64				Max() const { return _field()->Max(); }

private:
	inline	WgModValueField * 		_field() { return static_cast<WgModValueField*>(m_pField); }
	inline	const WgModValueField * _field() const { return static_cast<WgModValueField*>(m_pField); }
};


#endif //WG_MODVALUE_DOT_H
