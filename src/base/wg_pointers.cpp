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

namespace wg 
{
	
	
	class FreeWeakPtrHub;	
	
	
	Interface_p::Interface_p(Interface* pInterface )
		{
			m_pInterface = pInterface;
			if( pInterface )
			{
				m_pObj = pInterface->_object();
				if( m_pObj )
					m_pObj->_incRefCount();
			}
			else
				m_pObj = nullptr;				
		}

	
	Interface_wp::Interface_wp( Object * pObj, Interface * pInterface )
	{
		if( pObj && pInterface )
		{
			m_pInterface = pInterface;
	
			if( !pObj->m_pWeakPtrHub )
			{
				m_pHub = Base::_allocWeakPtrHub();
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

	Interface_wp::Interface_wp( Interface * pInterface )
	{
		if( pInterface )
		{
			m_pInterface = pInterface;
			Object * pObj = pInterface->_object();
	
			if( !pObj->m_pWeakPtrHub )
			{
				m_pHub = Base::_allocWeakPtrHub();
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

	
	
	Interface_wp::~Interface_wp()
	{
		if( m_pHub )
		{
			m_pHub->refCnt--;
	
			if( m_pHub->refCnt == 0 )
			{
				if( m_pHub->pObj )
					m_pHub->pObj->m_pWeakPtrHub = 0;
				Base::_freeWeakPtrHub(m_pHub);
			}
		}
	}
	
	
	
	void Interface_wp::copy( Interface_wp const & r)
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
					Base::_freeWeakPtrHub(m_pHub);
				}
			}
	
			m_pHub = r.m_pHub;
			if( m_pHub )
				m_pHub->refCnt++;
		}
	}

} // namespace wg
