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

#include <wg_presentablefield.h>

namespace wg 
{
	
	//____ Constructor _____________________________________________________________
	
	WgPresentableField::WgPresentableField( WgPresentableHolder * pHolder ) : WgField( pHolder )
	{
		_presenter()->addField(this);
	}
	
	//____ Destructor ______________________________________________________________
	
	WgPresentableField::~WgPresentableField()
	{
		_presenter()->removeField(this);
	}
	
	//____ setStyle() ______________________________________________________________
	
	void WgPresentableField::setStyle( const WgTextStyle_p& pStyle )
	{
		m_pStyle = pStyle;
		_presenter()->onStyleChange(this);
	}
	
	//____ clearStyle() ____________________________________________________________
	
	void WgPresentableField::clearStyle()
	{
		m_pStyle = 0;
		_presenter()->onStyleChange(this);
	}
	
	//____ setPresenter() __________________________________________________________
	
	void WgPresentableField::setPresenter( const WgTextPresenter_p& pPresenter )
	{
		if( pPresenter == m_pPresenter )
			return;
			
		_presenter()->removeField(this);
		m_pPresenter = pPresenter;
		_presenter()->addField(this);
	}
	
	//____ clearPresenter() ________________________________________________________
	
	void WgPresentableField::clearPresenter()
	{
		if( !m_pPresenter )
			return;
			
		_presenter()->removeField(this);
		m_pPresenter = 0;
		_presenter()->addField(this);
	}
	
	//____ setState() ______________________________________________________________
	
	void WgPresentableField::setState( WgState state )
	{
		if( state == m_state )
			return;
	
		WgState old = m_state;
		m_state = state;
		_presenter()->onStateChange( this, state, old );
	}
	
	//____ preferredSize() _________________________________________________________
	
	WgSize WgPresentableField::preferredSize() const
	{
		return _presenter()->preferredSize(this);
	}
	
	//____ matchingWidth() _________________________________________________________
	
	int WgPresentableField::matchingWidth( int height ) const
	{
		return _presenter()->matchingWidth(this, height);
	}
	
	//____ matchingHeight() ________________________________________________________
	
	int WgPresentableField::matchingHeight( int width ) const
	{
		return _presenter()->matchingHeight(this, width);
	}
	
	//____ coordToChar() ___________________________________________________________
	
	int WgPresentableField::coordToChar( WgCoord pos ) const
	{
		return _presenter()->coordToChar(this,pos);
	}
	
	//____ charToRect() ____________________________________________________________
	
	WgRect WgPresentableField::charToRect( int charOfs ) const
	{
		return _presenter()->charToRect(this, charOfs);
	}
	
	//____ onRefresh() _____________________________________________________________
	
	void WgPresentableField::onRefresh()
	{
		_presenter()->onRefresh(this);
	}
	
	//____ onNewSize() _____________________________________________________________
	
	void WgPresentableField::onNewSize( const WgSize& size )
	{
		if( size == m_size )
			return;
	
		m_size = size;
		_presenter()->onFieldResize(this,size);
	}
	
	//_____ onRender() _____________________________________________________________
	
	void  WgPresentableField::onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _clip )
	{
		_presenter()->renderField(this, pDevice, _canvas, _clip);
	}
	
	//____ rectForRange() __________________________________________________________
	
	WgRect  WgPresentableField::rectForRange( int ofs, int length ) const
	{
		_presenter()->rectForRange(this, ofs, length);
	}
	
	//____ tooltip() _______________________________________________________________
	
	WgString WgPresentableField::tooltip() const
	{
		return _presenter()->tooltip(this);
	}
	
	//____ getString() ___________________________________________________________________
	
	WgString WgPresentableField::getString() const
	{
		return WgString(&m_charBuffer);
	}
	
	//____ selectionBegin() ________________________________________________________
	
	int WgPresentableField::selectionBegin() const
	{
		return 0;
	}
	
	//____ selectionEnd() __________________________________________________________
	
	int WgPresentableField::selectionEnd() const
	{
		return 0;
	}

} // namespace wg
