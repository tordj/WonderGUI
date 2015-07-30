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

class WgCapsule;
typedef	WgStrongPtr<WgCapsule,WgContainerPtr>	WgCapsulePtr;
typedef	WgWeakPtr<WgCapsule,WgContainerPtr>	WgCapsuleWeakPtr;

class WgCapsuleHook;
typedef	WgHookTypePtr<WgCapsuleHook,WgHookPtr>	WgCapsuleHookPtr;

/**
 * @brief Base class for containers that only holds one child.
 *
 * WgCapsule is the base class for WonderGUI Capsules, minimalistic container
 * widgets that only can have one child.
 *
 * Capsules are typically used to "encapsulate" another widget (or branch of widgets) to affect its
 * geometry, appearance or behavior.
 *
 **/

class WgCapsuleHook : public WgHook
{
	friend class WgCapsule;
	friend class WgSizeCapsule;
	friend class WgShaderCapsule;

public:
	virtual bool			isInstanceOf( const char * pClassName ) const;
	virtual const char *	className( void ) const;
	static const char		CLASSNAME[];
	static WgCapsuleHookPtr	cast( const WgHookPtr& pInterface );

	// Standard Hook methods

	WgCoord			pos() const;
	WgSize			size() const;
	WgRect			geo() const;

	WgCoord			globalPos() const;
	WgRect			globalGeo() const;

	WgCapsulePtr 		parent() const;

protected:
	void			_requestRender();
	void			_requestRender( const WgRect& rect );
	void			_requestResize();

	WgHook *		_prevHook() const;
	WgHook *		_nextHook() const;
	WgContainer *	_parent() const;


	WgCapsule * 	m_pParent;
};



//____ WgCapsule ______________________________________________________________

class WgCapsule : public WgContainer
{
	friend class WgCapsuleHook;

public:
	bool		isInstanceOf( const char * pClassName ) const;
	const char *className( void ) const;
	static const char	CLASSNAME[];
	static WgCapsulePtr	cast( const WgObjectPtr& pObject );

	WgCapsuleHookPtr	setWidget( const WgWidgetPtr& pWidget );
	WgWidgetPtr			widget() { return m_hook.widget(); }
	bool				removeWidget( const WgWidgetPtr& pWidget );
	bool				clear();

	inline WgCapsuleHookPtr	firstHook() const { return static_cast<WgCapsuleHook*>(_firstHook()); }
	inline WgCapsuleHookPtr	lastHook() const { return static_cast<WgCapsuleHook*>(_lastHook()); }


	// Overloaded from WgWidget

	int				matchingHeight( int width ) const;
	int				matchingWidth( int height ) const;

	WgSize			preferredSize() const;

protected:
	WgCapsule();
	virtual ~WgCapsule() {}

	WgHook *		_firstHookWithGeo( WgRect& geo ) const;
	WgHook *		_nextHookWithGeo( WgRect& geo, WgHook * pHook ) const;

	WgHook *		_lastHookWithGeo( WgRect& geo ) const;
	WgHook *		_prevHookWithGeo( WgRect& geo, WgHook * pHook ) const;

	//

	void			_onCollectPatches( WgPatches& container, const WgRect& geo, const WgRect& clip );
	void			_onMaskPatches( WgPatches& patches, const WgRect& geo, const WgRect& clip, WgBlendMode blendMode );
	void			_onCloneContent( const WgWidget * _pOrg );
	void			_onNewSize( const WgSize& size );


	WgHook*			_firstHook() const;
	WgHook*			_lastHook() const;

	WgCapsuleHook	m_hook;

};

#endif //WG_CAPSULE_DOT_H
