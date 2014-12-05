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

#include <wg_pointers.h>

#include <wg_base.h>


WgObjectPtr::WgObjectPtr( const WgObjectWeakPtr& r )
{
	m_pObj = r.RawPtr();
	if( m_pObj )
		m_pObj->m_refCount++;
}


WgObjectWeakPtr::WgObjectWeakPtr( WgObject * pObj )
{
	if( pObj )
	{
		if( !pObj->m_pWeakPtrHub )
		{
			m_pHub = WgBase::AllocWeakPtrHub();
			m_pHub->refCnt = 1;
			m_pHub->pObj = pObj;
			pObj->m_pWeakPtrHub = m_pHub;
		}
		else
		{
			m_pHub = pObj->m_pWeakPtrHub;
			m_pHub->refCnt++;
		}
	}
	else
	{
		m_pHub = 0;
	}
};

WgObjectWeakPtr::WgObjectWeakPtr( const WgObjectPtr& pObject )
{
	WgObject * pObj = pObject.RawPtr();

	if( pObj )
	{
		if( !pObj->m_pWeakPtrHub )
		{
			m_pHub = WgBase::AllocWeakPtrHub();
			m_pHub->refCnt = 1;
			m_pHub->pObj = pObj;
			pObj->m_pWeakPtrHub = m_pHub;
		}
		else
		{
			m_pHub = pObj->m_pWeakPtrHub;
			m_pHub->refCnt++;
		}
	}
	else
	{
		m_pHub = 0;
	}
};

WgObjectWeakPtr::~WgObjectWeakPtr()
{
	if( m_pHub )
	{
		m_pHub->refCnt--;

		if( m_pHub->refCnt == 0 )
		{
			if( m_pHub->pObj )
				m_pHub->pObj->m_pWeakPtrHub = 0;
			WgBase::FreeWeakPtrHub(m_pHub);
		}
	}
}



void WgObjectWeakPtr::copy( WgObjectWeakPtr const & r)
{
	if( m_pHub != r.m_pHub )
	{
		if( m_pHub )
		{
			m_pHub->refCnt--;

			if( m_pHub->refCnt == 0 )
			{
				if( m_pHub->pObj )
					m_pHub->pObj->m_pWeakPtrHub = 0;
				WgBase::FreeWeakPtrHub(m_pHub);
			}
		}

		m_pHub = r.m_pHub;
		if( m_pHub )
			m_pHub->refCnt++;
	}
}


WgInterfaceWeakPtr::WgInterfaceWeakPtr( WgObject * pObj, WgInterface * pInterface )
{
	if( pObj && pInterface )
	{
		m_pInterface = pInterface;

		if( !pObj->m_pWeakPtrHub )
		{
			m_pHub = WgBase::AllocWeakPtrHub();
			m_pHub->refCnt = 1;
			m_pHub->pObj = pObj;
			pObj->m_pWeakPtrHub = m_pHub;
		}
		else
		{
			m_pHub = pObj->m_pWeakPtrHub;
			m_pHub->refCnt++;
		}
	}
	else
	{
		m_pHub = 0;
		m_pInterface = 0;
	}
};


WgInterfaceWeakPtr::~WgInterfaceWeakPtr()
{
	if( m_pHub )
	{
		m_pHub->refCnt--;

		if( m_pHub->refCnt == 0 )
		{
			if( m_pHub->pObj )
				m_pHub->pObj->m_pWeakPtrHub = 0;
			WgBase::FreeWeakPtrHub(m_pHub);
		}
	}
}



void WgInterfaceWeakPtr::copy( WgInterfaceWeakPtr const & r)
{
	m_pInterface = r.m_pInterface;

	if( m_pHub != r.m_pHub )
	{
		if( m_pHub )
		{
			m_pHub->refCnt--;

			if( m_pHub->refCnt == 0 )
			{
				if( m_pHub->pObj )
					m_pHub->pObj->m_pWeakPtrHub = 0;
				WgBase::FreeWeakPtrHub(m_pHub);
			}
		}

		m_pHub = r.m_pHub;
		if( m_pHub )
			m_pHub->refCnt++;
	}
}
