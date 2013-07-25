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
typedef	WgSmartPtr<WgCapsule,WgContainerPtr>	WgCapsulePtr;
typedef	WgWeakPtr<WgCapsule,WgContainerPtr>	WgCapsuleWeakPtr;



class WgCapsuleHook : public WgHook
{
	friend class WgCapsule;
	friend class WgSizeCapsule;
	friend class WgShaderCapsule;

public:
	const char *Type( void ) const;
	static const char * ClassType();

	// Standard Hook methods

	WgCoord			Pos() const;
	WgSize			Size() const;
	WgRect			Geo() const;

	WgCoord			ScreenPos() const;
	WgRect			ScreenGeo() const;

	WgCapsulePtr 		Parent() const;

protected:
	void			_requestRender();
	void			_requestRender( const WgRect& rect );
	void			_requestResize();

	WgHook *		_prevHook() const;
	WgHook *		_nextHook() const;
	WgContainer *	_parent() const;
	WgWidgetHolder*	_holder() const;

	WgCapsule * 	m_pParent;
};



//____ WgCapsule ______________________________________________________________

class WgCapsule : public WgContainer
{
	friend class WgCapsuleHook;

public:
	bool		IsInstanceOf( const char * pClassName ) const;
	const char *ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgCapsulePtr	Cast( const WgObjectPtr& pObject );

	WgHook *		SetChild( const WgWidgetPtr& pWidget );
	WgWidgetPtr		Child() { return m_hook.Widget(); }
	bool			RemoveChild( const WgWidgetPtr& pWidget );
	bool			Clear();

	inline WgCapsuleHook *	FirstHook() const { return static_cast<WgCapsuleHook*>(_firstHook()); }
	inline WgCapsuleHook *	LastHook() const { return static_cast<WgCapsuleHook*>(_lastHook()); }


	// Overloaded from WgWidget

	int				HeightForWidth( int width ) const;
	int				WidthForHeight( int height ) const;

	WgSize			PreferredSize() const;

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
