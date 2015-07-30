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

#ifndef WG_SHADERCAPSULE_DOT_H
#define WG_SHADERCAPSULE_DOT_H

#ifndef WG_CAPSULE_DOT_H
#	include <wg_capsule.h>
#endif

class WgShaderCapsule;
typedef	WgStrongPtr<WgShaderCapsule,WgCapsule_p>		WgShaderCapsule_p;
typedef	WgWeakPtr<WgShaderCapsule,WgCapsule_wp>	WgShaderCapsule_wp;

class WgShaderCapsule : public WgCapsule
{
public:
	static WgShaderCapsule_p	create() { return WgShaderCapsule_p(new WgShaderCapsule()); }

	bool		isInstanceOf( const char * pClassName ) const;
	const char *className( void ) const;
	static const char	CLASSNAME[];
	static WgShaderCapsule_p	cast( const WgObject_p& pObject );

	void		setColor( const WgColor& color);
	void		setTintMode( WgTintMode mode );
	void		setBlendMode( WgBlendMode mode );

	inline WgColor		color() { return m_tintColor; }
	inline WgBlendMode	blendMode() { return m_blendMode; }
	inline WgTintMode	tintMode() { return m_tintMode; }


protected:
	WgShaderCapsule();
	virtual ~WgShaderCapsule();
	virtual WgWidget* _newOfMyType() const { return new WgShaderCapsule(); };

	void		_renderPatches( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, WgPatches * _pPatches );
	void		_onCloneContent( const WgWidget * _pOrg );
	WgBlendMode _getBlendMode() const;

private:
	WgColor			m_tintColor;
	WgBlendMode		m_blendMode;
	WgTintMode		m_tintMode;

};

#endif //WG_SHADERCAPSULE_DOT_H
