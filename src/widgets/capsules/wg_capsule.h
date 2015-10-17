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

#ifndef WG_CAPSULE_DOT_H
#define WG_CAPSULE_DOT_H

#ifndef WG_CONTAINER_DOT_H
#	include <wg_container.h>
#endif

namespace wg 
{
	
	class Capsule;
	typedef	WgStrongPtr<Capsule,Container_p>	Capsule_p;
	typedef	WgWeakPtr<Capsule,Container_p>	Capsule_wp;
	
	class CapsuleHook;
	typedef	HookTypePtr<CapsuleHook,Hook_p>	CapsuleHook_p;
	
	/**
	 * @brief Base class for containers that only holds one child.
	 *
	 * Capsule is the base class for WonderGUI Capsules, minimalistic container
	 * widgets that only can have one child.
	 *
	 * Capsules are typically used to "encapsulate" another widget (or branch of widgets) to affect its
	 * geometry, appearance or behavior.
	 *
	 **/
	
	class CapsuleHook : public Hook
	{
		friend class Capsule;
		friend class SizeCapsule;
		friend class ShaderCapsule;
	
	public:
		virtual bool			isInstanceOf( const char * pClassName ) const;
		virtual const char *	className( void ) const;
		static const char		CLASSNAME[];
		static CapsuleHook_p	cast( const Hook_p& pInterface );
	
		// Standard MyHook methods
	
		Coord			pos() const;
		Size			size() const;
		Rect			geo() const;
	
		Coord			globalPos() const;
		Rect			globalGeo() const;
	
		Capsule_p 		parent() const;
	
	protected:
		void			_requestRender();
		void			_requestRender( const Rect& rect );
		void			_requestResize();
	
		Hook *		_prevHook() const;
		Hook *		_nextHook() const;
		Container *	_parent() const;
	
	
		Capsule * 	m_pParent;
	};
	
	
	
	//____ Capsule ______________________________________________________________
	
	class Capsule : public Container
	{
		friend class CapsuleHook;
	
	public:
		bool		isInstanceOf( const char * pClassName ) const;
		const char *className( void ) const;
		static const char	CLASSNAME[];
		static Capsule_p	cast( const Object_p& pObject );
	
		CapsuleHook_p	setWidget( const Widget_p& pWidget );
		Widget_p			widget() { return m_hook.widget(); }
		bool				removeWidget( const Widget_p& pWidget );
		bool				clear();
	
		inline CapsuleHook_p	firstHook() const { return static_cast<CapsuleHook*>(_firstHook()); }
		inline CapsuleHook_p	lastHook() const { return static_cast<CapsuleHook*>(_lastHook()); }
	
	
		// Overloaded from Widget
	
		int				matchingHeight( int width ) const;
		int				matchingWidth( int height ) const;
	
		Size			preferredSize() const;
	
	protected:
		Capsule();
		virtual ~Capsule() {}
	
		Hook *		_firstHookWithGeo( Rect& geo ) const;
		Hook *		_nextHookWithGeo( Rect& geo, Hook * pHook ) const;
	
		Hook *		_lastHookWithGeo( Rect& geo ) const;
		Hook *		_prevHookWithGeo( Rect& geo, Hook * pHook ) const;
	
		//
	
		void			_onCollectPatches( Patches& container, const Rect& geo, const Rect& clip );
		void			_onMaskPatches( Patches& patches, const Rect& geo, const Rect& clip, BlendMode blendMode );
		void			_onCloneContent( const Widget * _pOrg );
		void			_onNewSize( const Size& size );
	
	
		Hook*			_firstHook() const;
		Hook*			_lastHook() const;
	
		CapsuleHook	m_hook;
	
	};
	

} // namespace wg
#endif //WG_CAPSULE_DOT_H
