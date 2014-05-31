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
#ifndef	WG_STATICVALUEFIELD_DOT_H
#define WG_STATICVALUEFIELD_DOT_H

#ifndef WG_ISTATICVALUE_DOT_H
#	include <wg_istaticvalue.h>
#endif

#ifndef WG_CHARBUFFER_DOT_H
#	include <wg_charbuffer.h>
#endif

#ifndef WG_TEXTPROP_DOT_H
#	include <wg_textprop.h>
#endif

class WgStaticValueField;

//____ WgStaticValueHolder ___________________________________________________________

class WgStaticValueHolder
{
public:
	virtual void		_onFieldDirty( WgStaticValueField * pField ) = 0;
	virtual void		_onFieldResize( WgStaticValueField * pField ) = 0;
};

//____ WgStaticValueFieldBase ____________________________________________________________

class WgStaticValueFieldBase
{
public:
	WgStaticValueFieldBase();
	~WgStaticValueFieldBase();

	void					SetFormat( const WgValueFormatPtr& pFormat );
	inline WgValueFormatPtr	Format() const { return m_pFormat; }

	void					SetProperties( const WgTextpropPtr& pProp );
	void					ClearProperties();
	inline WgTextpropPtr	Properties() const { return m_pProp; }

	void					SetAlignment( WgOrigo alignment );
	inline WgOrigo			Alignment() const { return m_alignment; }

	void					SetAutoEllipsis(bool bAutoEllipsis);
	inline bool				AutoEllipsis() const { return m_bAutoEllipsis; }

protected:
	void					_setValue( Sint64 value );
	virtual void			_onFieldDirty();
	virtual void 			_onFieldResize();
	void					_regenText();
	void					_recalcSize();
	inline WgSize			_preferredSize() const { return m_size; }

	Sint64					m_value;
	
	WgValueFormatPtr		m_pFormat;
	WgTextpropPtr			m_pProp;
	WgOrigo					m_alignment;
	bool					m_bAutoEllipsis;
	WgCharBuffer			m_charBuffer;
	WgSize					m_size;
};

//____ WgStaticValueField ______________________________________________________

class WgStaticValueField : public WgStaticValueFieldBase, public WgIStaticValue
{
	friend class WgStaticValueHolder;
protected:
	void 	_setHolder( WgStaticValueHolder * pHolder ) { m_pHolder = pHolder; }
	void	_onFieldDirty() { m_pHolder->_onFieldDirty(this); }
	void	_onFieldResize() { m_pHolder->_onFieldResize(this); }

	WgStaticValueHolder *	m_pHolder;
};


#endif //WG_STATICVALUEFIELD_DOT_H
