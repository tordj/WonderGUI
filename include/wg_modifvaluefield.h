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

#ifndef WG_STATICVALUEFIELD_DOT_H
#	include <wg_staticvaluefield.h>
#endif

#ifndef WG_IMODIFVALUE_DOT_H
#	include <wg_imodifvalue.h>
#endif

class WgModifValueField;

//____ WgModifValueHolder ___________________________________________________________

class WgModifValueHolder
{
public:
	virtual void		_onFieldDirty( WgModifValueField * pField ) = 0;
	virtual void		_onFieldResize( WgModifValueField * pField ) = 0;
	virtual void		_onValueModified( WgModifValueField * pField ) = 0;
};


//____ WgModifValueFieldBase ____________________________________________________________

class WgModifValueFieldBase : public WgStaticValueFieldBase
{
public:
	WgModifValueFieldBase();
	~WgModifValueFieldBase();

	// IModifValue methods

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
	virtual void		_onValueModified();

	Sint64				m_minValue;
	Sint64				m_maxValue;
	float				m_fraction;
};

//____ WgModifValueField ______________________________________________________

class WgModifValueField : public WgModifValueFieldBase, public WgIModifValue
{
	friend class WgModifValueHolder;
protected:
	void 	_setHolder( WgModifValueHolder * pHolder ) { m_pHolder = pHolder; }
	void	_onFieldDirty() { m_pHolder->_onFieldDirty(this); }
	void	_onFieldResize() { m_pHolder->_onFieldResize(this); }
	void	_onValueModified() { m_pHolder->_onValueModified(this); }

	WgModifValueHolder *	m_pHolder;
};



#endif //WG_MODIFVALUEFIELD_DOT_H
