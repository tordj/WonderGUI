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



#include "testwidget.h"
#include <wg_gfxdevice.h>
#include <wg_msgrouter.h>
#include <wg_base.h>
#include <wg_msg.h>

namespace wg
{
	const TypeInfo TestWidget::TYPEINFO = { "TestWidget", &Widget::TYPEINFO };

	//____ constructor ____________________________________________________________

	TestWidget::TestWidget()
	{
		m_bOpaque = false;
		m_bPointsInitialized = false;
		m_bAnimated = false;
	}

	//____ Destructor _____________________________________________________________

	TestWidget::~TestWidget()
	{
		
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& TestWidget::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ preferredSize() __________________________________________________________

	Size TestWidget::preferredSize() const
	{
		return Size(200,200);
	}

	//____ start() _________________________________________________________________

	void TestWidget::start()
	{
        if( !m_tickRouteId )
            m_tickRouteId = Base::msgRouter()->addRoute( MsgType::Tick, this );
	}

	//____ stop() __________________________________________________________________

	void TestWidget::stop()
	{
        if( m_tickRouteId )
        {
            Base::msgRouter()->deleteRoute( m_tickRouteId );
            m_tickRouteId = 0;
        }
	}

	//____ _cloneContent() _______________________________________________________

	void TestWidget::_cloneContent( const Widget * _pOrg )
	{
	}

	//____ _render() _____________________________________________________________

	void TestWidget::_render( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window )
	{
 //       pDevice->setBlendMode(BlendMode::Invert);
        
        

		if( m_bPointsInitialized )
		{
		
			pDevice->drawLine( _canvas.pos().px(), Coord(_canvas.x + _canvas.w, _canvas.y + _canvas.h ).px(), Color(255,000,000), 10.f );

			for( int i = 0 ; i < 2 ; i+=2 )
			{
				pDevice->drawLine( _canvas.pos() + m_coord[i], _canvas.pos() + m_coord[i+1], Color(000,255,000), 15.5f );
			}
		}


 //       pDevice->setBlendMode(BlendMode::Blend);
        
	}

	//____ _receive() ______________________________________________________________

	void TestWidget::_receive( Msg* pMsg )
	{
		switch( pMsg->type() )
		{
			case MsgType::Tick:
			{
                auto pTick = static_cast<TickMsg*>(pMsg);
				
				RectI area( 0,0, m_size );
				
				
				if( !m_bPointsInitialized )
				{
					for( int i = 0 ; i < 2 ; i+=2 )
					{
						CoordI& c = m_coord[i];
						
						c.x = 0;
						c.y = 0;

						c = m_coord[i+1];

						c.x = 100000;
						c.y = 100000;
						
					}
					m_bPointsInitialized = true;
				}
				
				
				for( int i = 0 ; i < 2 ; i++ )
				{
					CoordI& c = m_coord[i];
					
					if( c.x < area.x )
						c.x = area.x;
						
					if( c.y < area.y )
						c.y = area.y;

					if( c.x > area.x + area.w )
						c.x = area.x + area.w;

					if( c.y > area.y + area.h )
						c.y = area.y + area.h;					

					
					
					if( (c.y == area.y) && (c.x < area.x + area.w) )
						c.x++;
					else if( (c.x == area.x + area.w) && (c.y < area.y + area.h) )
						c.y++;
					else if( (c.y == area.y + area.h) && (c.x > area.x) )
						c.x--;
					else
						c.y--;
				}
				
				_requestRender();

			}
			break;

			default:
			break;
		}
	}

} // namespace wg
