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

#include <wg_hookptr.h>
#include <wg_hook.h>
#include <wg_base.h>

WgHook_p::WgHook_p( WgHook * pHook )
{
	if( pHook )
	{
		if( !pHook->m_pPtrHub )
		{
			m_pHub = WgBase::allocHookPtrHub();
			m_pHub->refCnt = 1;
			m_pHub->pObj = pHook;
			pHook->m_pPtrHub = m_pHub;
		}
		else
		{
			m_pHub = pHook->m_pPtrHub;
			m_pHub->refCnt++;
		}
	}
	else
	{
		m_pHub = 0;
	}
};

WgHook_p::~WgHook_p()
{
	if( m_pHub )
	{
		m_pHub->refCnt--;

		if( m_pHub->refCnt == 0 )
		{
			if( m_pHub->pObj )
				m_pHub->pObj->m_pPtrHub = 0;
			WgBase::freeHookPtrHub(m_pHub);
		}
	}
}

void WgHook_p::copy( WgHook_p const & r)
{
	if( m_pHub != r.m_pHub )
	{
		if( m_pHub )
		{
			m_pHub->refCnt--;

			if( m_pHub->refCnt == 0 )
			{
				if( m_pHub->pObj )
					m_pHub->pObj->m_pPtrHub = 0;
				WgBase::freeHookPtrHub(m_pHub);
			}
		}

		m_pHub = r.m_pHub;
		if( m_pHub )
			m_pHub->refCnt++;
	}
}
