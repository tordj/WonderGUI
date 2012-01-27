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


class WgMonotainer : public WgGizmo, public WgGizmoContainer
{
public:
	WgMonotainer();
	~WgMonotainer();

	WgHook *	SetChild( WgGizmoContainer * pGizmo );
	WgGizmo *		Child();
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

	bool			IsView() const { return false; }
	bool			IsContainer() const { return true; }

	WgGizmoContainer * CastToContainer() { return this; }
	const WgGizmoContainer * CastToContainer() const { return this; }

	WgGizmo *		CastToGizmo() { return this; }

	// Overloaded from container

	WgGizmo *		FindGizmo( const WgCoord& ofs, WgSearchMode mode );

private:

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

		WgWidget*		GetRoot() { return 0; }			// Should in the future not return a widget, but a gizmo.

	protected:
		Hook( WgMonotainer * pParent ) : m_pParent(pParent) {}

		void			_requestRender() { if( !m_bHidden ) m_pParent->RequestRender(); }
		void			_requestRender( const WgRect& rect ) { if( !m_bHidden ) m_pParent->RequestRender(rect); }
		void			_requestResize() { if( !m_bHidden ) m_pParent->RequestResize(); }

		WgHook *		_prevHook() const { return 0; }
		WgHook *		_nextHook() const { return 0; }
		WgGizmoParent * _parent() const { return m_pParent; }

		WgMonotainer * 	m_pParent;

	};

	WgHook *		_firstHookWithGeo( WgRect& geo ) const;
	WgHook *		_nextHookWithGeo( WgRect& geo, WgHook * pHook ) const;

	void			_renderPatches( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, WgPatches * _pPatches, Uint8 _layer )
									{ WgGizmoContainer::_renderPatches( pDevice, _canvas, _window, _pPatches, _layer ); }

	void			_onCollectPatches( WgPatches& container, const WgRect& geo, const WgRect& clip );
	void			_onMaskPatches( WgPatches& patches, const WgRect& geo, const WgRect& clip );
	void			_onCloneContent( const WgGizmo * _pOrg );
	void			_onNewSize( const WgSize& size );

	void			_onEnable();
	void			_onDisable();
	inline bool 	_onAlphaTest( const WgCoord& ofs ) { WgGizmoContainer::_onAlphaTest(ofs); }


	WgHook*	_firstHook() const;
	WgHook*	_lastHook() const;

	Hook			m_hook;

};

#endif //WG_MONOTAINER_DOT_H
