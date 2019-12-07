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

		bool					isInstanceOf( const char * pClassName ) const override;
		const char *			className( void ) const override;
		static const char		CLASSNAME[];
		static ShaderCapsule_p	cast( Object * pObject );

		//.____ Appearance _________________________________________________

		void				setTintColor( Color color, BlendMode operation = BlendMode::Replace );
		void				setRenderMode( BlendMode mode );

		inline Color		tintColor() { return m_tintColor; }
		inline BlendMode	renderMode() { return m_renderMode; }
		inline BlendMode	tintMode() { return m_tintMode; }


	protected:
		ShaderCapsule();
		virtual ~ShaderCapsule();
		virtual Widget* _newOfMyType() const override { return new ShaderCapsule(); };

		void		_render( GfxDevice * pDevice, const RectI& _canvas, const RectI& _window ) override;
		void		_cloneContent( const Widget * _pOrg ) override;
		BlendMode _getRenderMode() const;


	private:
		class SlotAccess : public CBasicSlot { friend class ShaderCapsule; };
		SlotAccess * _slot() { return static_cast<SlotAccess*>(&slot); }
		const SlotAccess * _slot() const { return static_cast<const SlotAccess*>(&slot); }

		Color			m_tintColor;
		BlendMode		m_tintMode;
		BlendMode		m_renderMode;

	};


} // namespace wg
#endif //WG_SHADERCAPSULE_DOT_H
