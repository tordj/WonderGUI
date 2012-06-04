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

#ifndef WG_MONOTAINER_DOT_H
#define WG_MONOTAINER_DOT_H

#ifndef WG_GIZMO_CONTAINER_DOT_H
#	include <wg_gizmo_container.h>
#endif


class WgMonotainer : public WgGizmoContainer
{
public:
	WgMonotainer() : m_hook(this) {}
	~WgMonotainer() {}

	WgHook *		SetChild( WgGizmo * pGizmo );
	WgGizmo *		Child() { return m_hook.Gizmo(); }
	bool			DeleteChild();
	WgGizmo *		ReleaseChild();

	bool			DeleteChild( WgGizmo * pGizmo );
	WgGizmo *		ReleaseChild( WgGizmo * pGizmo );

	bool			DeleteAllChildren();
	bool			ReleaseAllChildren();


	// Overloaded from WgGizmo

	int				HeightForWidth( int width ) const;
	int				WidthForHeight( int height ) const;

	WgSize			DefaultSize() const;

protected:

	class Hook : public WgHook
	{
		friend class WgMonotainer;

	public:
		// Standard Hook methods

		WgCoord			Pos() const { return m_pParent->Pos(); }
		WgSize			Size() const { 	return m_pParent->Size(); }
		WgRect			Geo() const { return m_pParent->Geo(); }

		WgCoord			ScreenPos() const { return m_pParent->ScreenPos(); }
		WgRect			ScreenGeo() const { return m_pParent->ScreenGeo(); }

		WgMonotainer* 	Parent() const { return m_pParent; }

	protected:
		Hook( WgMonotainer * pParent ) : m_pParent(pParent) {}

		void			_requestRender() { if( !m_bHidden ) m_pParent->_requestRender(); }
		void			_requestRender( const WgRect& rect ) { if( !m_bHidden ) m_pParent->_requestRender(rect); }
		void			_requestResize() { if( !m_bHidden ) m_pParent->_requestResize(); }

		WgHook *		_prevHook() const { return 0; }
		WgHook *		_nextHook() const { return 0; }
		WgGizmoParent * _parent() const { return m_pParent; }

		WgMonotainer * 	m_pParent;

	};

	WgHook *		_firstHookWithGeo( WgRect& geo ) const;
	WgHook *		_nextHookWithGeo( WgRect& geo, WgHook * pHook ) const;

	WgHook *		_lastHookWithGeo( WgRect& geo ) const;
	WgHook *		_prevHookWithGeo( WgRect& geo, WgHook * pHook ) const;

	//

	void			_onCollectPatches( WgPatches& container, const WgRect& geo, const WgRect& clip );
	void			_onMaskPatches( WgPatches& patches, const WgRect& geo, const WgRect& clip, WgBlendMode blendMode );
	void			_onCloneContent( const WgGizmo * _pOrg );
	void			_onNewSize( const WgSize& size );


	WgHook*			_firstHook() const;
	WgHook*			_lastHook() const;

	Hook			m_hook;

};

#endif //WG_MONOTAINER_DOT_H
