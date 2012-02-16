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

#ifndef WG_GIZMO_SHADER_DOT_H
#define WG_GIZMO_SHADER_DOT_H

#ifndef WG_MONOTAINER_DOT_H
#	include <wg_monotainer.h>
#endif


class WgGizmoShader : public WgMonotainer
{
public:
	WgGizmoShader();
	~WgGizmoShader();

	virtual const char *Type( void ) const;
	static const char * GetMyType();
	virtual WgGizmo * NewOfMyType() const { return new WgGizmoShader(); };

	void		SetColor( const WgColor& color);
	void		SetTintMode( WgTintMode mode );
	void		SetBlendMode( WgBlendMode mode );

	inline WgColor		Color() { return m_tintColor; }
	inline WgBlendMode	BlendMode() { return m_blendMode; }
	inline WgTintMode	TintMode() { return m_tintMode; }


protected:
	void	_renderPatches( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, WgPatches * _pPatches, Uint8 _layer );
	void	_onCloneContent( const WgGizmo * _pOrg );

private:
	WgColor			m_tintColor;
	WgBlendMode		m_blendMode;
	WgTintMode		m_tintMode;

};

#endif //WG_GIZMO_SHADER_DOT_H
