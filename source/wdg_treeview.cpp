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


#include	<wdg_treeview.h>
#include	<wdg_menu.h>
#include	<wg_gfx.h>
#include	<wg_item.h>
#include	<assert.h>

static const char	Wdg_Type[] = {"TordJ/TreeView"};

//____ NewOfMyType() __________________________________________________________

WgWidget * Wdg_TreeView::NewOfMyType() const
{
	return new Wdg_TreeView;
}


//____ Init() _________________________________________________________________

void Wdg_TreeView::Init( void )
{
	m_indentation			= 16;
	m_minEntryHeight		= 16;
	m_bMultiSelect			= false;
	m_bDynamicItems			= false;
	m_multiSelectModifier	= WG_MODKEY_CTRL;


	for( int i = 0 ; i < WG_MAX_BUTTONS ; i++ )
		m_pContextMenu[i] = 0;

	/*
	m_viewStartY			= 0;
	m_viewStartSlot			= 0;
	m_viewStartSlotPixelOfs = 0;
*/
	m_expanderSize			= WgSize(0,0);

	for( int i = 0 ; i < WG_NB_MODES ; i++ )
		m_entryBgFlags[i] = false;
	m_entryBgFlags[WG_MODE_SELECTED] = true;

	m_bNormalBg = m_bDisabledBg = m_bMarkedBg = false;
}

//____ ~Wdg_TreeView() ____________________________________________________________

Wdg_TreeView::~Wdg_TreeView( void )
{
}


//____ Type() _________________________________________________________________

const char * Wdg_TreeView::Type( void ) const
{
	return GetMyType();
}

const char * Wdg_TreeView::GetMyType( void )
{
	return Wdg_Type;
}


//____ SetContextMenu() _______________________________________________________

void Wdg_TreeView::SetContextMenu( int button, Wdg_Menu * pMenu )
{
	assert( button <= WG_MAX_BUTTONS );
	if( button > WG_MAX_BUTTONS )
		return;

	m_pContextMenu[button-1] = pMenu;
}



//____ DoMyOwnActionRespond() __________________________________________________

void Wdg_TreeView::DoMyOwnActionRespond( WgInput::UserAction _action, int _button_key, const WgActionDetails& _info, const WgInput& _inputObj )
{
	int x = _info.x;
	int y = _info.y;

	Abs2local( &x, &y );

	switch( _action )
	{
		case	WgInput::POINTER_ENTER:
			break;

		case	WgInput::POINTER_EXIT:
			break;

		case WgInput::BUTTON_PRESS:
		{
			Sint32 ofs;
			WgTreeEntry * pEntry = FindEntryAtViewOfs( y, &ofs );


			if( _button_key == 1 )
			{
				if( pEntry )
				{
					WgRect expander = GetExpanderRect(pEntry);
					if( expander.contains( x + m_viewPixOfsX, ofs ) )
					{
						if( pEntry->IsOpen() )
							CloseEntry( pEntry );
						else
							OpenEntry( pEntry );
					}
					else
					{
						if( _info.modifier == m_multiSelectModifier )
						{
							if( pEntry->IsSelected() )
								UnselectEntry( pEntry );
							else
								SelectEntry( pEntry );
						}
						else
						{
							UnselectAllEntries();
							SelectEntry( pEntry );
						}
					}
				}
				else
				{
					if( _info.modifier != m_multiSelectModifier )
						UnselectAllEntries();
				}
			}
			else
			{
				if( pEntry && m_pContextMenu[_button_key-1] && Root()->IsRootWidget() )
				{
					if( !pEntry->IsSelected() )
					{
						UnselectAllEntries();
						SelectEntry( pEntry );
					}

					m_pContextMenu[_button_key-1]->Open( (Wdg_Root*)Root(), _info.x, _info.y );
				}
			}

		}
			break;

		case WgInput::BUTTON_RELEASE:
			break;

		case WgInput::BUTTON_RELEASE_OUTSIDE:
			break;

        default:
            break;

	}
}

//____ DoMyOwnMarkTest() ___________________________________________________

bool Wdg_TreeView::DoMyOwnMarkTest( int _x, int _y )
{
	return true;
}

//____ GetExpanderRect() ______________________________________________________

WgRect Wdg_TreeView::GetExpanderRect( WgTreeEntry * pEntry )
{
	WgRect r;

	if( pEntry->bOpen )
		r = m_pExpanderOpen->GetSize();
	else
		r = m_pExpanderClosed->GetSize();

	r.x = pEntry->GetLevel() * m_indentation;
	r.x += (m_indentation-r.w)/2;

	r.y = (GetEntryHeight(pEntry)-r.h)/2;

	return r;
}

//____ FindEntryAtViewOfs() ___________________________________________________

WgTreeEntry * Wdg_TreeView::FindEntryAtViewOfs( Sint32 ofs, Sint32 * wpOfsInside )
{
	ofs += m_viewPixOfsY;

	WgTreeEntry * pEntry = m_entries.getFirst();

	Sint32	height = 0;

	if( pEntry )
		height = GetEntryHeight(pEntry);

	while( pEntry && (ofs - height > 0) )
	{
		ofs -= height;
		pEntry = GetNextDisplayedEntry(pEntry);

		if( pEntry )
			height = GetEntryHeight(pEntry);
	}

	if( wpOfsInside )
		* wpOfsInside = ofs;

	return pEntry;
}

//____ DoMyOwnRender() ________________________________________________________

void Wdg_TreeView::DoMyOwnRender( const WgRect& _window, const WgRect& _clip, Uint8 _layer )
{
	int yOfs = _window.y -m_viewPixOfsY;

	WgTreeEntry * pEntry = m_entries.getFirst();
	if( pEntry == 0 )
		return;

	int	height = GetEntryHeight(pEntry);

	while( pEntry && yOfs + height < _clip.y )
	{
		yOfs += height;
		pEntry = GetNextDisplayedEntry(pEntry);
		if( pEntry )
			height = GetEntryHeight(pEntry);
	}

	while( pEntry && yOfs < _clip.y + _clip.h )
	{
		int xOfs = _window.x - m_viewPixOfsX;
		xOfs += pEntry->GetLevel() * m_indentation;

		Uint32 height = GetEntryHeight( pEntry );

		if( pEntry->HasChildren() )
		{
			WgBlock	block;

			if( pEntry->bOpen )
				block = m_pExpanderOpen->GetBlock( WG_MODE_NORMAL );		// TODO: Take right block
			else
				block = m_pExpanderClosed->GetBlock( WG_MODE_NORMAL );		// TODO: Take right block

			WgRect dest = GetExpanderRect(pEntry);
			dest.x += _window.x - m_viewPixOfsX;
			dest.y += yOfs;

			WgGfx::clipBlitBlock( _clip, block, dest );
		}

		xOfs += m_indentation;
		WgRect	dest( xOfs, yOfs, pEntry->width, height );

		if( m_pEntryBg && m_entryBgFlags[pEntry->mode] )
			WgGfx::clipBlitBlock( _clip, m_pEntryBg->GetBlock( pEntry->mode ), dest );

		pEntry->pItem->Render( dest, _clip );

		yOfs += height;
		pEntry = GetNextDisplayedEntry(pEntry);
	}
}


//____ GetNextDisplayedEntry() ________________________________________________

WgTreeEntry * Wdg_TreeView::GetNextDisplayedEntry( WgTreeEntry * pEntry )
{
	if( pEntry->bOpen && pEntry->HasChildren() )
		return pEntry->GetFirstChild();

	if( pEntry->GetNextSibling() != 0 )
		return pEntry->GetNextSibling();

	while( pEntry->GetParent() )
	{
		pEntry = pEntry->GetParent();
		if( pEntry->GetNextSibling() )
			return pEntry->GetNextSibling();
	}

	return 0;
}

//____ GetEntryHeight() _______________________________________________________

Uint32 Wdg_TreeView::GetEntryHeight( WgTreeEntry * pEntry )
{
	Uint32 height = pEntry->pItem->Height();
	if( height < m_minEntryHeight )
		height = m_minEntryHeight;

	if( pEntry->HasChildren() && height < (unsigned int) m_expanderSize.h )
		height = m_expanderSize.h;

	return height;
}

//____ GetEntryOfs() __________________________________________________________

Uint32 Wdg_TreeView::GetEntryOfs( WgTreeEntry * pEntry )
{
	WgTreeEntry * p = m_entries.getFirst();
	if( p == 0 )
		return 0;

	Uint32	ofs = 0;

	while( p && p != pEntry  )
	{
		ofs += GetEntryHeight(p);
		p = GetNextDisplayedEntry(p);
	}

	return ofs;
}



//____ DoMyOwnCloning() _______________________________________________________

void	Wdg_TreeView::DoMyOwnCloning( WgWidget * _pClone, const WgWidget * _pCloneRoot,
																const WgWidget * _pBranchRoot )
{
	Wdg_TreeView * pClone = (Wdg_TreeView *) _pClone;

}

//____ SetExpanderSource() ____________________________________________________

void Wdg_TreeView::SetExpanderSource( const WgBlockSetPtr& pGfxOpen, const WgBlockSetPtr& pGfxClosed )
{
	m_pExpanderOpen = pGfxOpen;
	m_pExpanderClosed = pGfxClosed;

	WgSize sz = WgSize::max( pGfxOpen->GetSize(), pGfxClosed->GetSize() );

	if( sz != m_expanderSize )
	{
		RefreshSize(0);				//TODO: NOT ENOUGH! Needs to refresh size on all levels because of possible height change!
		m_expanderSize = sz;
	}

	RequestRender();
}

//____ SetIndentation() _______________________________________________________

void Wdg_TreeView::SetIndentation( Uint8 nPixels )
{
	m_indentation = nPixels;

	//TODO: Recalculate width for all entries, enclosedWidth and m_contentWidth.

	RequestRender();
}

//____ SetMinEntryHeight() ____________________________________________________

void Wdg_TreeView::SetMinEntryHeight( Uint32 nPixels )
{
	m_minEntryHeight = nPixels;

	//TODO: Recalculate height for content and all enclosedHeight.

	RequestRender();
}

//____ SetMultiSelect() _______________________________________________________

void Wdg_TreeView::SetMultiSelect( bool bMultiSelect )
{
	m_bMultiSelect = bMultiSelect;
}

//____ SetMultiSelectModifier() _______________________________________________

void Wdg_TreeView::SetMultiSelectModifier( WgModifierKeys modifier )
{
	m_multiSelectModifier = modifier;
}

//____ SetEntryBackground() ___________________________________________________

void Wdg_TreeView::SetEntryBackground( const WgBlockSetPtr& pGfxEntryBg, bool bNormalBg,
										bool bDisabledBg, bool bMarkedBg )
{
	m_pEntryBg = pGfxEntryBg;
	m_entryBgFlags[WG_MODE_NORMAL]		= bNormalBg;
	m_entryBgFlags[WG_MODE_DISABLED]	= bDisabledBg;
	m_entryBgFlags[WG_MODE_MARKED]		= bMarkedBg;

	RequestRender();
}


//____ AddEntry() _____________________________________________________________

WgTreeEntry * Wdg_TreeView::AddEntry( long id, WgTreeEntry * pParent, WgTreeEntry * pBefore, WgItem * pItem )
{
	WgTreeEntry * p = new WgTreeEntry( id, pParent, pItem->Width(), pItem );

	WgChain<WgTreeEntry> * pChain;

	if( pParent == 0 )
		pChain = &m_entries;
	else
		pChain = &pParent->m_children;

	pChain->push_back(p);

	if( pBefore )
		if( pBefore->GetParent() == pParent )
			p->moveBefore( pBefore );

	RefreshSize(pParent);
	RequestRender();		//TODO: Optimize, only call if needed.
	return p;
}

//____ MoveEntry() ____________________________________________________________

bool Wdg_TreeView::MoveEntry( WgTreeEntry * pEntry, WgTreeEntry * pParent, WgTreeEntry * pBefore )
{
	WgTreeEntry * pOldParent = pEntry->GetParent();

	if( pParent != pOldParent )
	{
		pEntry->disconnect();

		WgChain<WgTreeEntry> * pChain;

		if( pParent == 0 )
			pChain = &m_entries;
		else
			pChain = &pParent->m_children;

		pChain->push_back(pEntry);

		RefreshSize(pParent);
		RefreshSize(pOldParent);
	}

	if( pBefore )
		if( pBefore->GetParent() == pParent )
			pEntry->moveBefore( pBefore );

	RequestRender();		//TODO: Optimize, only call if needed.

	return true;
}




//____ ReplaceEntry() __________________________________________________________

bool Wdg_TreeView::ReplaceEntry( WgTreeEntry * pEntry, WgItem * _pItem )
{
	//TODO: new item might have different size than previous... we need to update!


	if( !pEntry )
		return false;

	delete pEntry->pItem;
	pEntry->pItem = _pItem;

	RequestRender();						//TODO: Optimize, only call if necessary and for needed screensection.

	return true;
}


//____ DeleteEntry() __________________________________________________________

bool Wdg_TreeView::DeleteEntry( WgTreeEntry * pEntry )
{
	if( pEntry->mode == WG_MODE_SELECTED )
		UnselectEntry(pEntry);

	WgTreeEntry * pParent = pEntry->pParent;
	delete pEntry;

	RefreshSize(pParent);
	RequestRender();						//TODO: Optimize, only call if necessary and for needed screensection.
	return true;
}


//____ CalcSize() _____________________________________________________________

WgSize Wdg_TreeView::CalcSize( WgChain<WgTreeEntry>& chain )
{
	WgSize sz(0,0);

	WgTreeEntry * p = chain.getFirst();

	// Calculate content width/height.

	Uint32 xOfs;

	if( p )
		xOfs = (p->GetLevel()+1) * m_indentation;	// Same offset for them all

	while( p )
	{
		sz.h += GetEntryHeight(p);
		int width = p->width + xOfs;

		if( width > sz.w )
			sz.w = width;

		if( p->bOpen )
		{
			sz.h += p->childrenHeight;

			if( p->childrenWidth + xOfs > (unsigned int) sz.w )
				sz.w = p->childrenWidth;
		}
		p = p->getNext();
	}
	return sz;
}

//____ RefreshSize() __________________________________________________________

void Wdg_TreeView::RefreshSize( WgTreeEntry * p )
{
	WgSize sz;

	if( p == 0 )
	{
		sz = CalcSize( m_entries );
		SetContentSize( sz.w, sz.h );
	}
	else
	{
		sz = CalcSize( p->m_children );

		// Propagate changes through ancestry as long as the folders are open
		// (thus size of our content affecting size of ancestry).

		if( sz.w != p->childrenWidth )
		{
			p->childrenWidth = sz.w;
			p->childrenHeight = sz.h;

			if( p->bOpen )
				RefreshSize( p->pParent );
		}
		else if( sz.h != (int) p->childrenHeight )
		{
			// No changes to width so we only need to propagate the change
			// in height through our ancestry until we reach a closed folder.

			int diff = sz.h - p->childrenHeight;

			while( p )
			{
				p->childrenHeight += diff;
				if( !p->bOpen )
					return;

				p = p->pParent;
			}

			if( !p )
				SetContentSize( m_contentWidth, m_contentHeight + diff );
		}
	}
}


//____ OpenEntry() ____________________________________________________________

bool Wdg_TreeView::OpenEntry( WgTreeEntry * pEntry )
{
	if( pEntry->m_children.getFirst() == 0 )
		return false;

	if( pEntry->bOpen )
		return true;

	pEntry->bOpen = true;
	Emit( WgSignal::TreeEntryOpened(), pEntry );

	RefreshSize(pEntry->pParent);
	RequestRender();						//TODO: Optimize, only call if necessary and for needed screensection.

	//TODO: Possibly move view if below the opened entry.

	return true;
}

//____ CloseEntry() ___________________________________________________________

bool Wdg_TreeView::CloseEntry( WgTreeEntry * pEntry )
{
	if( pEntry->m_children.getFirst() == 0 )
		return false;

	if( !pEntry->bOpen )
		return true;

	pEntry->bOpen = false;
	Emit( WgSignal::TreeEntryClosed(), pEntry );

	RefreshSize(pEntry->pParent);
	RequestRender();						//TODO: Optimize, only call if necessary and for needed screensection.

	//TODO: Deselect selected entries contained within?
	//TODO: Possibly move view if below the opened entry.

	return true;
}



//____ EnableEntry() __________________________________________________________

bool Wdg_TreeView::EnableEntry( WgTreeEntry * pEntry )
{
	if( pEntry->mode != WG_MODE_DISABLED )
		return true;

	pEntry->mode = WG_MODE_NORMAL;

	RequestRender();						//TODO: Optimize, only call if necessary and for needed screensection.
	return true;
}

//____ DisableEntry() _________________________________________________________

bool Wdg_TreeView::DisableEntry( WgTreeEntry * pEntry )
{
	if( pEntry->mode == WG_MODE_SELECTED )
		UnselectEntry(pEntry);

	pEntry->mode = WG_MODE_DISABLED;

	RequestRender();						//TODO: Optimize, only call if necessary and for needed screensection.
	return true;
}

//____ SelectEntry() __________________________________________________________

bool Wdg_TreeView::SelectEntry( WgTreeEntry * pEntry )
{
	if( pEntry->mode == WG_MODE_DISABLED )
		return false;

	if( pEntry->mode == WG_MODE_SELECTED )
		return true;

	// If not multiselect we need to deselect all previously selected entries.

	if( !m_bMultiSelect )
		UnselectAllEntries();

	pEntry->mode = WG_MODE_SELECTED;
	Emit( WgSignal::TreeEntrySelected(), pEntry );

	RequestRender();						//TODO: Optimize, only call if necessary and for needed screensection.
	return true;
}

//____ UnselectEntry() ________________________________________________________

bool Wdg_TreeView::UnselectEntry( WgTreeEntry * pEntry )
{
	if( pEntry->mode == WG_MODE_DISABLED )
		return false;

	if( pEntry->mode == WG_MODE_NORMAL || pEntry->mode == WG_MODE_MARKED )
		return true;

	pEntry->mode = WG_MODE_NORMAL;
	Emit( WgSignal::TreeEntryUnselected(), pEntry );

	RequestRender();						//TODO: Optimize, only call if necessary and for needed screensection.
	return true;
}


//____ SelectAllEntries() _____________________________________________________

void Wdg_TreeView::SelectAllEntries()
{
	WgTreeEntry * p = GetFirstEntry();

	while( p )
	{
		if( !p->IsSelected() )
			SelectEntry(p);
		p = p->GetNext();
	}

	RequestRender();			// We want one big rectangle instead of many small...
}


//____ UnselectAllEntries() ___________________________________________________

void Wdg_TreeView::UnselectAllEntries()
{
	WgTreeEntry * p = GetFirstSelectedEntry();

	while( p )
	{
		UnselectEntry(p);
		p = GetNextSelectedEntry(p);
	}

	RequestRender();			// We want one big rectangle instead of many small...
}

//____ DeleteAllEntries() _____________________________________________________

void Wdg_TreeView::DeleteAllEntries()
{
	m_entries.clear();
	RequestRender();			// We want one big rectangle instead of many small...
}

//____ OpenAllEntries() _______________________________________________________

void Wdg_TreeView::OpenAllEntries()
{
	WgTreeEntry * p = GetFirstEntry();

	while( p )
	{
		if( !p->IsOpen() )
			OpenEntry(p);
		p = p->GetNext();
	}

	RequestRender();			// We want one big rectangle instead of many small...
}

//____ CloseAllEntries() ______________________________________________________

void Wdg_TreeView::CloseAllEntries()
{
	WgTreeEntry * p = GetFirstEntry();

	while( p )
	{
		if( p->IsOpen() )
			CloseEntry(p);
		p = p->GetNext();
	}

	RequestRender();			// We want one big rectangle instead of many small...
}

//____ EnableAllEntries() _____________________________________________________

void Wdg_TreeView::EnableAllEntries()
{
	WgTreeEntry * p = GetFirstEntry();

	while( p )
	{
		if( p->IsDisabled() )
			EnableEntry(p);
		p = p->GetNext();
	}

	RequestRender();			// We want one big rectangle instead of many small...
}

//____ DisableAllEntries() ____________________________________________________

void Wdg_TreeView::DisableAllEntries()
{
	WgTreeEntry * p = GetFirstEntry();

	while( p )
	{
		if( !p->IsDisabled() )
			DisableEntry(p);
		p = p->GetNext();
	}

	RequestRender();			// We want one big rectangle instead of many small...
}

//____ GotoEntry() ____________________________________________________________

void Wdg_TreeView::GotoEntry( WgTreeEntry * pEntry )
{
	// First we need to open up all ancestors...

	WgTreeEntry * p = pEntry->GetParent();
	while( p )
	{
		if( !p->IsOpen() )
			OpenEntry( p );
		p = p->GetParent();
	}

	// Get offset of entry

	Uint32 ofsY = GetEntryOfs( pEntry );
	Uint32 height = GetEntryHeight( pEntry );

	// Scroll into view

	if( ViewPixelOfsY() > ofsY )
		SetViewPixelOfsY( ofsY );

	if( ViewPixelOfsY() + ViewPixelLenY() < ofsY + height )
		SetViewPixelOfsY( ofsY + height - ViewPixelLenY() );



}




//____ () _____________________________________________________________________

WgTreeEntry * Wdg_TreeView::GetFirstSelectedEntry()
{
	WgTreeEntry * p = GetFirstEntry();
	if( p == 0 )
		return 0;

	if( p->mode == WG_MODE_SELECTED )
		return p;

	return GetNextSelectedEntry(p);
}

//____ () _____________________________________________________________________

WgTreeEntry * Wdg_TreeView::GetLastSelectedEntry()
{
	WgTreeEntry * p = GetLastEntry();
	if( p == 0 )
		return 0;

	if( p->mode == WG_MODE_SELECTED )
		return p;

	return GetPrevSelectedEntry(p);
}

//____ () _____________________________________________________________________

WgTreeEntry * Wdg_TreeView::GetNextSelectedEntry( WgTreeEntry * pEntry )
{
	pEntry = pEntry->GetNext();
	while( pEntry )
	{
		if( pEntry->mode == WG_MODE_SELECTED )
			return pEntry;

		pEntry = pEntry->GetNext();
	}

	return 0;
}

//____ () _____________________________________________________________________

WgTreeEntry * Wdg_TreeView::GetPrevSelectedEntry( WgTreeEntry * pEntry )
{
	pEntry = pEntry->GetPrev();
	while( pEntry )
	{
		if( pEntry->mode == WG_MODE_SELECTED )
			return pEntry;

		pEntry = pEntry->GetPrev();
	}

	return 0;
}

//____ () _____________________________________________________________________

WgTreeEntry * Wdg_TreeView::FindEntry( long id )
{
	WgTreeEntry * p = GetFirstEntry();

	while( p )
	{
		if( p->GetID() == id )
			return p;
		p = p->GetNext();
	}

	return 0;
}




//____ WgTreeEntry::Constructor _______________________________________________

WgTreeEntry::WgTreeEntry( long _id, WgTreeEntry * _pParent, Uint16 _width, WgItem * _pItem )
{
	id				= _id;
	pParent			= _pParent;
	bOpen			= false;
	mode			= WG_MODE_NORMAL;
	width			= _width;
	childrenWidth	= 0;
	childrenHeight	= 0;
	pItem			= _pItem;
}

//____ WgTreeEntry::Destructor ________________________________________________

WgTreeEntry::~WgTreeEntry()
{
	if( pItem )
		delete pItem;
}

//____ WgTreeEntry::GetPrev() _________________________________________________

WgTreeEntry * WgTreeEntry::GetPrev()
{
	WgTreeEntry * p = getPrev();			// Check for previous sibling.

	if( p )
	{
		// We had a sibling, recurse through its last children.

		while( p->m_children.getLast() )
			p = p->m_children.getLast();

		return p;							// Return sibling or last child/grandchild of it.
	}

	return pParent;
}

//____ WgTreeEntry::GetNext() _________________________________________________

WgTreeEntry * WgTreeEntry::GetNext()
{
	if( m_children.getFirst() )
		return m_children.getFirst();		// We had a child, return it.

	if( getNext() )
		return getNext();					// We had a sibling, return it.

	// Iterate through our ancestry and return first next sibling
	// of them that we can find.

	WgTreeEntry * p = pParent;
	while( p != 0 )
	{
		if( p->getNext() )
			return p->getNext();

		p = p->pParent;
	}

	return 0;
}

//____ WgTreeEntry::GetLevel() ________________________________________________

Uint32 WgTreeEntry::GetLevel()
{
	Uint32 level = 0;

	WgTreeEntry * p = pParent;

	while( p )
	{
		level++;
		p = p->pParent;
	}

	return level;
}

