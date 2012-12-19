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

#ifndef WG_MONOPANEL_DOT_H
#define WG_MONOPANEL_DOT_H

#ifndef WG_PANEL_DOT_H
#	include <wg_panel.h>
#endif


class WgMonoPanel : public WgPanel
{
public:
	WgMonoPanel() : m_hook(this) {}
	~WgMonoPanel() {}

	WgHook *		SetChild( WgWidget * pWidget );
	WgWidget *		Child() { return m_hook.Widget(); }
	bool			DeleteChild();
	WgWidget *		ReleaseChild();

	bool			DeleteChild( WgWidget * pWidget );
	WgWidget *		ReleaseChild( WgWidget * pWidget );

	bool			DeleteAllChildren();
	bool			ReleaseAllChildren();


	// Overloaded from WgWidget

	int				HeightForWidth( int width ) const;
	int				WidthForHeight( int height ) const;

	WgSize			DefaultSize() const;

protected:

	class Hook : public WgHook
	{
		friend class WgMonoPanel;

	public:

		const char *Type( void ) const;
		static const char * ClassType();

		// Standard Hook methods

		WgCoord			Pos() const { return m_pParent->Pos(); }
		WgSize			Size() const { 	return m_pParent->Size(); }
		WgRect			Geo() const { return m_pParent->Geo(); }

		WgCoord			ScreenPos() const { return m_pParent->ScreenPos(); }
		WgRect			ScreenGeo() const { return m_pParent->ScreenGeo(); }

		WgMonoPanel* 	Parent() const { return m_pParent; }

	protected:
		Hook( WgMonoPanel * pParent ) : m_pParent(pParent) {}

		void			_requestRender() { if( m_bVisible ) m_pParent->_requestRender(); }
		void			_requestRender( const WgRect& rect ) { if( m_bVisible ) m_pParent->_requestRender(rect); }
		void			_requestResize() { if( m_bVisible ) m_pParent->_requestResize(); }

		WgHook *		_prevHook() const { return 0; }
		WgHook *		_nextHook() const { return 0; }
		WgWidgetHolder * _parent() const { return m_pParent; }

		WgMonoPanel * 	m_pParent;

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

	Hook			m_hook;

};

#endif //WG_MONOPANEL_DOT_H
