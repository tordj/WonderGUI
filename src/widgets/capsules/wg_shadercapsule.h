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

namespace wg 
{
	
	class ShaderCapsule;
	typedef	StrongPtr<ShaderCapsule,Capsule_p>		ShaderCapsule_p;
	typedef	WeakPtr<ShaderCapsule,Capsule_wp>	ShaderCapsule_wp;
	
	class ShaderCapsule : public Capsule
	{
	public:
		static ShaderCapsule_p	create() { return ShaderCapsule_p(new ShaderCapsule()); }
	
		bool		isInstanceOf( const char * pClassName ) const;
		const char *className( void ) const;
		static const char	CLASSNAME[];
		static ShaderCapsule_p	cast( const Object_p& pObject );
	
		void		setTintColor( Color color, BlendMode operation = BlendMode::Replace );
		void		setRenderMode( BlendMode mode );
	
		inline Color		tintColor() { return m_tintColor; }
		inline BlendMode	renderMode() { return m_renderMode; }
		inline BlendMode	tintMode() { return m_tintMode; }
	
	
	protected:
		ShaderCapsule();
		virtual ~ShaderCapsule();
		virtual Widget* _newOfMyType() const { return new ShaderCapsule(); };
	
		void		_renderPatches( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, Patches * _pPatches );
		void		_cloneContent( const Widget * _pOrg );
		BlendMode _getRenderMode() const;
	
	private:
		Color			m_tintColor;
		BlendMode		m_tintMode;
		BlendMode		m_renderMode;
	
	};
	

} // namespace wg
#endif //WG_SHADERCAPSULE_DOT_H
