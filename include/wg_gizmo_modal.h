/*=========================================================================

                         >>> WonderGUI <<<

  This file is part of Tord Jansson's WonderGUI Graphics Toolkit
  and copyright (c) Tord Jansson, Sweden [tord.jansson@gmail.com].

                            -----------

  The WonderGUI Graphics Toolkit is free software you can redistribute
  this file and/or modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation either
  version 2 of the License, or (at your option) any later version.

                            -----------

  The WonderGUI Graphics Toolkit is also available for use in commercial
  closed-source projects under a separate license. Interested parties
  should contact Tord Jansson [tord.jansson@gmail.com] for details.

=========================================================================*/

#ifndef WG_GIZMO_MODAL_DOT_H
#define WG_GIZMO_MODAL_DOT_H

class WgModalHook : public WgGizmoHook, protected WgLink
{
	friend class WgGizmoModal;

public:

	void	Top();								// Put us ontop of all our silbings.

	bool	SetGeo( const WgRect& geometry, WgLocation origo = WG_NORTHWEST );
	bool	SetGeo( const WgCord& ofs, WgLocation origo = WG_NORTHWEST );

	bool	SetOfs( const WgCord& ofs );
	bool	SetOfsX( int x );
	bool	SetOfsY( int y );

	bool	SetSize( WgSize sz );
	bool	SetWidth( int width );
	bool	SetHeight( int height );

	bool	Move( const WgCord& ofs );
	bool	MoveX( int x );
	bool	MoveY( int y );




	// Standard Hook methods

	inline WgCord		Pos() const { return m_realGeo.pos(); }
	inline WgSize		Size() const { 	return m_realGeo.size(); }
	inline WgRect		Geo() const { return m_realGeo; }

	WgCord		ScreenPos() const;
	WgRect		ScreenGeo() const;

	inline WgModalHook *	PrevHook() const { return _prev(); }
	inline WgModalHook *	NextHook() const { return _next(); }

	WgGizmoContainer* Parent() const;

	WgWidget*	GetRoot();			// Should in the future not return a widget, but a gizmo.

protected:
	// TODO: Constructor should in the future call SetHook() on Gizmo, once we are totally rid of widgets...

	PROTECTED_LINK_METHODS( WgModalHook );

	WgModalHook( WgGizmo * pGizmo, WgGizmoModal * pParent );

	bool		_refreshRealGeo();	// Return false if we couldn't get exactly the requested (floating) geometry.

	void		RequestRender();
	void		RequestRender( const WgRect& rect );
	void		RequestResize();

	void		_castDirtRecursively( const WgRect& parentGeo, const WgRect& clip, WgRectLink * pDirtIn, WgRectChain * pDirtOut );
	void		_renderDirtyRects( WgGfxDevice * pDevice, const WgCord& parentPos, Uint8 _layer );


	WgGizmoHook *	_prevHook() const;
	WgGizmoHook *	_nextHook() const;


	WgGizmoModal * m_pParent;

	WgRect		m_realGeo;			// Gizmos geo relative parent

	WgLocation	m_origo;
	WgRect		m_placementGeo;		// Gizmos geo relative anchor and hotspot.

};



class WgGizmoModal : public WgGizmo, public WgGizmoContainer
{
public:
	WgGizmoModal();
	~WgGizmoModal();

	virtual const char *Type( void ) const;
	static const char * GetMyType();

	WgGizmoHook *	SetBaseGizmo( WgGizmo * pGizmo );
	WgGizmo *		BaseGizmo();
	bool			DeleteBaseGizmo();
	WgGizmo *		ReleaseBaseGizmo();


	WgModalHook *	AddModalGizmo( WgGizmo * pGizmo, const WgRect& geometry, WgLocation origo = WG_NORTHWEST );
	WgModalHook *	AddModalGizmo( WgGizmo * pGizmo, const WgCord& pos, WgLocation origo = WG_NORTHWEST );

	bool			DeleteAllModalGizmos();
	bool			ReleaseAllModalGizmos();

	bool			DeleteGizmo( WgGizmo * pGizmo );
	bool			ReleaseGizmo( WgGizmo * pGizmo );

	bool			DeleteAllGizmos();
	bool			ReleaseAllGizmos();

	WgModalHook *	FirstModalGizmo();
	WgModalHook *	LastModalGizmo();


	// Overloaded from WgGizmo

	int				HeightForWidth( int width ) const;
	int				WidthForHeight( int height ) const;

	WgSize			MinSize() const;
	WgSize			BestSize() const;
	WgSize			MaxSize() const;

	bool			IsView() const { return false; }
	bool			IsContainer() const { return true; }

	WgGizmoContainer * CastToContainer() { return this; }
	const WgGizmoContainer * CastToContainer() const { return this; }


	// Overloaded from container

	WgGizmo *		FindGizmo( const WgCord& ofs, WgSearchMode mode );

private:

	class BaseHook : public WgGizmoHook
	{
		friend class WgGizmoModal;

	public:
		// Standard Hook methods

		inline WgCord		Pos() const { return m_pParent->Pos(); }
		inline WgSize		Size() const { 	return m_pParent->Size(); }
		inline WgRect		Geo() const { return m_pParent->Geo(); }

		inline WgCord		ScreenPos() const { return m_pParent->ScreenPos(); }
		inline WgRect		ScreenGeo() const { return m_pParent->ScreenGeo(); }

		inline WgGizmoContainer* Parent() const { return m_pParent; }

		inline WgWidget*	GetRoot() { return 0; }			// Should in the future not return a widget, but a gizmo.

	protected:
		BaseHook( WgGizmoModal * pParent ) : m_pParent(pParent) {}

		void		RequestRender();
		void		RequestRender( const WgRect& rect );
		void		RequestResize();

		void		_castDirtRecursively( const WgRect& parentGeo, const WgRect& clip, WgRectLink * pDirtIn, WgRectChain * pDirtOut );
		void		_renderDirtyRects( WgGfxDevice * pDevice, const WgCord& parentPos, Uint8 _layer );


		WgGizmoHook *	_prevHook() const { return 0; }
		WgGizmoHook *	_nextHook() const { return m_pParent->FirstModalChild(); }

		WgGizmoModal * m_pParent;
	};



	void			_onCollectRects( WgRectChain& rects, const WgRect& geo, const WgRect& clip );
	void			_onMaskRects( WgRectChain& rects, const WgRect& geo, const WgRect& clip );
	void			_onCloneContent( const WgGizmo * _pOrg );
	void			_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip, Uint8 _layer );
	void			_onNewSize( const WgSize& size );
	void			_onAction( WgInput::UserAction action, int button_key, const WgActionDetails& info, const WgInput& inputObj );
	bool			_onAlphaTest( const WgCord& ofs );

	inline void		_onEnable() { WgGizmoContainer::OnEnable(); }
	inline void		_onDisable() { WgGizmoContainer::OnDisable(); }

	void			_onRequestRender( const WgRect& rect, const WgFlexHook * pHook );	// rect is in our coordinate system.

	WgGizmo*		_castToGizmo() { return this; }

	void			_castDirtyRect( const WgRect& geo, const WgRect& clip, WgRectLink * pDirtIn, WgRectChain* pDirtOut );
	void			_renderDirtyRects( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, Uint8 _layer );
	void			_clearDirtyRects();


	WgGizmoHook*	_firstHook();		// Fist Hook returned is the normal child, then follows the modal ones.
	WgGizmoHook*	_lastHook();		//


	BaseHook				m_baseHook;
	WgChain<WgModalHook>	m_modalHooks;		// First modal gizmo lies at the bottom.

	WgColor					m_dimColor;


};

#endif //WG_GIZMO_MODAL_DOT_H
