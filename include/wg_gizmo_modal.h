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
	bool	SetGeo( const WgCord& pos, WgLocation origo = WG_NORTHWEST );

	bool	SetOfs( const WgCord& ofs );
	bool	SetOfsX( int x );
	bool	SetOfsY( int y );

	bool	Move( const WgCord& ofs );
	bool	MoveX( int x );
	bool	MoveY( int y );



	// Standard Hook methods

	inline WgCord		Pos() const { return m_realGeo.pos(); }
	inline WgSize		Size() const { 	return m_realGeo.size(); }
	inline WgRect		Geo() const { return m_realGeo; }

	WgCord		ScreenPos() const;
	WgRect		ScreenGeo() const;

	inline WgModalHook *	PrevHook() const { return Prev(); }
	inline WgModalHook *	NextHook() const { return Next(); }

	WgGizmoContainer* Parent() const;

	WgWidget*	GetRoot();			// Should in the future not return a widget, but a gizmo.

protected:
	// TODO: Constructor should in the future call SetHook() on Gizmo, once we are totally rid of widgets...

	LINK_METHODS( WgModalHook );

	WgModalHook( WgGizmo * pGizmo, WgGizmoModal * pParent );

	bool		RefreshRealGeo();	// Return false if we couldn't get exactly the requested (floating) geometry.

	void		RequestRender();
	void		RequestRender( const WgRect& rect );
	void		RequestResize();

	void		_castDirtRecursively( const WgRect& parentGeo, const WgRect& clip, WgDirtyRect * pDirtIn, WgDirtyRectObj * pDirtOut );
	void		_renderDirtyRects( WgGfxDevice * pDevice, const WgCord& parentPos, Uint8 _layer );


	WgGizmoHook *	_prevHook() const;
	WgGizmoHook *	_nextHook() const;


	WgGizmoModal * m_pParent;

	WgRect		m_realGeo;			// Gizmos geo relative parent

	WgLocation	m_origo;
	WgRect		m_placementGeo;	// Gizmos geo relative anchor and hotspot.

};



class WgGizmoModal : public WgGizmo, public WgGizmoContainer
{
public:
	WgGizmoModal();
	~WgGizmoModal();

	virtual const char *Type( void ) const;
	static const char * GetMyType();

	bool			SetNormalChild( WgGizmo * pGizmo );
	WgGizmo *		NormalChild();
	bool			DeleteNormalChild();
	WgGizmo *		ReleaseNormalChild();


	WgModalHook *	AddModalChild( WgGizmo * pGizmo, const WgRect& geometry, WgLocation origo = WG_NORTHWEST );
	WgModalHook *	AddModalChild( WgGizmo * pGizmo, const WgCord& pos, WgLocation origo = WG_NORTHWEST );
	bool			DeleteModalChild( WgGizmo * pGizmo );
	bool			ReleaseModalChild( WgGizmo * pGizmo );

	WgModalHook *	FirstModalChild();
	WgModalHook *	LastModalChild();
	

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

	class NormalHook : public WgGizmoHook
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
		NormalHook( WgGizmo * pGizmo, WgGizmoModal * pParent );

		void		RequestRender();
		void		RequestRender( const WgRect& rect );
		void		RequestResize();

		void		_castDirtRecursively( const WgRect& parentGeo, const WgRect& clip, WgDirtyRect * pDirtIn, WgDirtyRectObj * pDirtOut );
		void		_renderDirtyRects( WgGfxDevice * pDevice, const WgCord& parentPos, Uint8 _layer );


		WgGizmoHook *	_prevHook() const { return 0; }
		WgGizmoHook *	_nextHook() const { return m_pParent->FirstModalChild(); }


		WgGizmoModal * m_pParent;
	};



	void			OnCollectRects( WgDirtyRectObj& rects, const WgRect& geo, const WgRect& clip );
	void			OnMaskRects( WgDirtyRectObj& rects, const WgRect& geo, const WgRect& clip );
	void			OnCloneContent( const WgGizmo * _pOrg );
	void			OnRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip, Uint8 _layer );
	void			OnNewSize( const WgSize& size );
	void			OnAction( WgInput::UserAction action, int button_key, const WgActionDetails& info, const WgInput& inputObj );
	bool			OnAlphaTest( const WgCord& ofs );

	inline void		OnEnable() { WgGizmoContainer::OnEnable(); }
	inline void		OnDisable() { WgGizmoContainer::OnDisable(); }

	void			OnRequestRender( const WgRect& rect, const WgFlexHook * pHook );	// rect is in our coordinate system.

	WgGizmo*		_castToGizmo() { return this; }

	void			_castDirtyRect( const WgRect& geo, const WgRect& clip, WgDirtyRect * pDirtIn, WgDirtyRectObj* pDirtOut );
	void			_renderDirtyRects( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, Uint8 _layer );
	void			_clearDirtyRects();


	WgGizmoHook*	_firstHook() const { return FirstHook(); }
	WgGizmoHook*	_lastHook() const { return LastHook(); }


	NormalHook				m_normalHook;
	WgChain<WgModalHook>	m_modalHooks;

};

#endif //WG_GIZMO_MODAL_DOT_H
