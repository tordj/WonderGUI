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
	
	PresentableField::PresentableField( PresentableHolder * pHolder ) : Field( pHolder )
	{
		_presenter()->addField(this);
	}
	
	//____ Destructor ______________________________________________________________
	
	PresentableField::~PresentableField()
	{
		_presenter()->removeField(this);
	}
	
	//____ setStyle() ______________________________________________________________
	
	void PresentableField::setStyle( const TextStyle_p& pStyle )
	{
		m_pStyle = pStyle;
		_presenter()->onStyleChange(this);
	}
	
	//____ clearStyle() ____________________________________________________________
	
	void PresentableField::clearStyle()
	{
		m_pStyle = 0;
		_presenter()->onStyleChange(this);
	}
	
	//____ setPresenter() __________________________________________________________
	
	void PresentableField::setPresenter( const TextPresenter_p& pPresenter )
	{
		if( pPresenter == m_pPresenter )
			return;
			
		_presenter()->removeField(this);
		m_pPresenter = pPresenter;
		_presenter()->addField(this);
	}
	
	//____ clearPresenter() ________________________________________________________
	
	void PresentableField::clearPresenter()
	{
		if( !m_pPresenter )
			return;
			
		_presenter()->removeField(this);
		m_pPresenter = 0;
		_presenter()->addField(this);
	}
	
	//____ setState() ______________________________________________________________
	
	void PresentableField::setState( State state )
	{
		if( state == m_state )
			return;
	
		State old = m_state;
		m_state = state;
		_presenter()->onStateChange( this, state, old );
	}
	
	//____ preferredSize() _________________________________________________________
	
	Size PresentableField::preferredSize() const
	{
		return _presenter()->preferredSize(this);
	}
	
	//____ matchingWidth() _________________________________________________________
	
	int PresentableField::matchingWidth( int height ) const
	{
		return _presenter()->matchingWidth(this, height);
	}
	
	//____ matchingHeight() ________________________________________________________
	
	int PresentableField::matchingHeight( int width ) const
	{
		return _presenter()->matchingHeight(this, width);
	}
	
	//____ coordToChar() ___________________________________________________________
	
	int PresentableField::coordToChar( Coord pos ) const
	{
		return _presenter()->coordToChar(this,pos);
	}
	
	//____ charToRect() ____________________________________________________________
	
	Rect PresentableField::charToRect( int charOfs ) const
	{
		return _presenter()->charToRect(this, charOfs);
	}
	
	//____ onRefresh() _____________________________________________________________
	
	void PresentableField::onRefresh()
	{
		_presenter()->onRefresh(this);
	}
	
	//____ onNewSize() _____________________________________________________________
	
	void PresentableField::onNewSize( const Size& size )
	{
		if( size == m_size )
			return;
	
		m_size = size;
		_presenter()->onFieldResize(this,size);
	}
	
	//_____ onRender() _____________________________________________________________
	
	void  PresentableField::onRender( GfxDevice * pDevice, const Rect& _canvas, const Rect& _clip )
	{
		_presenter()->renderField(this, pDevice, _canvas, _clip);
	}
	
	//____ rectForRange() __________________________________________________________
	
	Rect  PresentableField::rectForRange( int ofs, int length ) const
	{
		return _presenter()->rectForRange(this, ofs, length);
	}
	
	//____ tooltip() _______________________________________________________________
	
	String PresentableField::tooltip() const
	{
		return _presenter()->tooltip(this);
	}
	
	//____ getString() ___________________________________________________________________
	
	String PresentableField::getString() const
	{
		return String(&m_charBuffer);
	}
	
	//____ selectionBegin() ________________________________________________________
	
	int PresentableField::selectionBegin() const
	{
		return 0;
	}
	
	//____ selectionEnd() __________________________________________________________
	
	int PresentableField::selectionEnd() const
	{
		return 0;
	}

} // namespace wg
