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
#ifndef	WG_MODIFVALUEFIELD_DOT_H
#define WG_MODIFVALUEFIELD_DOT_H

#ifndef WG_VALUEFIELD_DOT_H
#	include <wg_valuefield.h>
#endif


class WgModifValueField;
typedef	WgCompStrongPtr<WgModifValueField,WgValueFieldPtr>		WgModifValueFieldPtr;
typedef	WgCompWeakPtr<WgModifValueField,WgValueFieldWeakPtr>		WgModifValueFieldWeakPtr;

//____ WgModifValueHolder ___________________________________________________________

class WgModifValueHolder : public WgValueHolder
{
public:
	virtual void		_onValueModified( WgModifValueField * pField ) = 0;
};


//____ WgModifValueField _____________________________________________________________

class WgModifValueField : public WgValueField
{
	friend class WgModifValueHolder;
public:
	WgModifValueField();
	~WgModifValueField();

	virtual bool			IsInstanceOf( const char * pClassName ) const;
	virtual const char *	ClassName( void ) const;
	static const char		CLASSNAME[];
	static WgModifValueFieldPtr	Cast( const WgComponentPtr& pComponent );

	WgModifValueFieldPtr	Ptr() { return WgModifValueFieldPtr(this); }


	void				Set( int value );
	void				Set( Sint64 value );
	void				Set( float value );
	void				Set( double value );

	inline Sint64		Value() const { return m_value; }
	
	void				SetFraction( float fraction );	
	inline float		Fraction() const { return m_fraction; }

	bool				SetRange( Sint64 min, Sint64 max );
	inline Sint64		Min() const { return m_minValue; }
	inline Sint64		Max() const { return m_maxValue; }

protected:
	void 				_setHolder( WgModifValueHolder * pHolder ) { m_pHolder = pHolder; }
	void				_onValueModified() { static_cast<WgModifValueHolder*>(m_pHolder)->_onValueModified(this); }


	Sint64				m_minValue;
	Sint64				m_maxValue;
	float				m_fraction;
};

#endif //WG_MODIFVALUEFIELD_DOT_H
