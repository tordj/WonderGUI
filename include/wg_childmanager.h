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

#ifndef WG_CHILDMANAGER_DOT_H
#define WG_CHILDMANAGER_DOT_H

#ifndef	WG_GIZMO_COLLECTION_DOT_H
#	include <wg_gizmo_collection.h>
#endif

#ifndef WG_GIZMO_HOOK_DOT_H
#	include <wg_gizmo_hook.h>
#endif

#ifndef WG_CHAIN_DOT_H
#	include <wg_chain.h>
#endif


//____ WgChildHook() __________________________________________________________

class WgChildHook : public WgGizmoHook, private WgLink
{
friend class WgChildManager;

public:
	WgCord	Pos() const;
	WgSize	Size() const;
	WgRect	Geo() const;
	WgCord	ScreenPos() const;
	WgRect	ScreenGeo() const;

	WgGizmoHook * PrevHook() const;
	WgGizmoHook * NextHook() const;

	WgChildHook * Prev() const;
	WgChildHook * Next() const;

	// Needs to be here for now since Emitters are inherrited by Widgets. Shouldn't be hooks business in the future...

	WgEmitter* 	GetEmitter();
	WgWidget*	GetRoot();			// Should in the future not return a widget, but a gizmo.

protected:
	WgChildHook( WgGizmo * pGizmo, WgChildManager * pManager );

	void	RequestRender();
	void	RequestRender( const WgRect& rect );
	void	RequestResize();

	bool	RequestFocus();
	bool	ReleaseFocus();

private:

};



//____ WgChildManager() _______________________________________________________

class WgChildManager : public WgGizmoCollection
{
public:
	WgChildManager();
	virtual ~WgChildManager();

	inline WgGizmoHook *	FirstHook() const { return m_hooks.getFirst(); };
	inline WgGizmoHook *	LastHook() const { return m_hooks.getLast(); };

	inline WgChildHook *	FirstCastHook() const { return m_hooks.getFirst(); };
	inline WgChildHook *	LastCastHook() const { return m_hooks.getLast(); };


private:

	WgChain<WgChildHook>	m_hooks;
};

#endif //WG_CHILDMANAGER_DOT_H