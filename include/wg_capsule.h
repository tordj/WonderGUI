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


class WgCapsule : public WgContainer
{
public:
	~WgCapsule() {}

	WgHook *		SetChild( WgWidget * pWidget );
	WgWidget *		Child() { return m_hook.Widget(); }
	bool			DeleteChild();
	WgWidget *		ReleaseChild();

	bool			DeleteChild( WgWidget * pWidget );
	WgWidget *		ReleaseChild( WgWidget * pWidget );

	bool			DeleteAllChildren();
	bool			ReleaseAllChildren();


	bool			IsCapsule() const;
	WgCapsule *		CastToCapsule();
	const WgCapsule *	CastToCapsule() const;

	// Overloaded from WgWidget

	int				HeightForWidth( int width ) const;
	int				WidthForHeight( int height ) const;

	WgSize			PreferredSize() const;

protected:
	WgCapsule();

	class CapsuleHook : public WgHook
	{
		friend class WgCapsule;

	public:

		const char *Type( void ) const;
		static const char * ClassType();

		// Standard Hook methods

		WgCoord			Pos() const { return m_pParent->Pos(); }
		WgSize			Size() const { 	return m_pParent->Size(); }
		WgRect			Geo() const { return m_pParent->Geo(); }

		WgCoord			ScreenPos() const { return m_pParent->ScreenPos(); }
		WgRect			ScreenGeo() const { return m_pParent->ScreenGeo(); }

		WgCapsule* 		Parent() const { return m_pParent; }

	protected:
		void			_requestRender() { m_pParent->_requestRender(); }
		void			_requestRender( const WgRect& rect ) { m_pParent->_requestRender(rect); }
		void			_requestResize() { m_pParent->_requestResize(); }

		WgHook *		_prevHook() const { return 0; }
		WgHook *		_nextHook() const { return 0; }
		WgContainer *	_parent() const { return m_pParent; }
		WgWidgetHolder*	_holder() const { return m_pParent; }

		WgCapsule * 	m_pParent;

	};

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

	CapsuleHook		m_hook;

};

#endif //WG_CAPSULE_DOT_H
