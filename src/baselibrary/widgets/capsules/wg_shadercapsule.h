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
#pragma once

#include <wg_capsule.h>

namespace wg
{

	class ShaderCapsule;
	typedef	StrongPtr<ShaderCapsule>		ShaderCapsule_p;
	typedef	WeakPtr<ShaderCapsule>	ShaderCapsule_wp;


	//____ ShaderCapsule ______________________________________________________
	/**
	* @brief	A widget that sets the tint color and blend mode of a single child.
	*
	* The ShaderCapsule sets the tint color and blend mode, which is inherited by its
	* sub-widgets branch during the rendering process.
	*
	* This can for example be used to create fading effects when parts of the UI (dis)appears.
	*
	*/

	class ShaderCapsule : public Capsule
	{
	public:
		//.____ Creation __________________________________________

		static ShaderCapsule_p	create() { return ShaderCapsule_p(new ShaderCapsule()); }

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Appearance _________________________________________________

		void				setTintColor( HiColor color, BlendMode operation = BlendMode::Replace );
		void				setRenderMode( BlendMode mode );

		inline HiColor		tintColor() { return m_tintColor; }
		inline BlendMode	renderMode() { return m_renderMode; }
		inline BlendMode	tintMode() { return m_tintMode; }


	protected:
		ShaderCapsule();
		virtual ~ShaderCapsule();

		void		_render( GfxDevice * pDevice, const RectSPX& _canvas, const RectSPX& _window ) override;
		BlendMode	_getRenderMode() const;


	private:

		HiColor			m_tintColor;
		BlendMode		m_tintMode;
		BlendMode		m_renderMode;

	};


} // namespace wg
#endif //WG_SHADERCAPSULE_DOT_H
