
#include <wg_rulerlabels.h>
#include <wg_gfxdevice.h>

namespace wg 
{
	
	const char RulerLabels::CLASSNAME[] = {"RulerLabels"};
	
	
	//____ Constructor ____________________________________________________________
	
	RulerLabels::RulerLabels()
	{
		m_direction = Direction::Right;
	}
	
	//____ Destructor _____________________________________________________________
	
	RulerLabels::~RulerLabels()
	{
	}
	
	//____ isInstanceOf() _________________________________________________________
	
	bool RulerLabels::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return Widget::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * RulerLabels::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	RulerLabels_p RulerLabels::cast( const Object_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return RulerLabels_p( static_cast<RulerLabels*>(pObject.rawPtr()) );
	
		return 0;
	}
	
	
	//____ addLabel() ____________________________________________________________
	
	void RulerLabels::addLabel( const CharSeq& text, const TextStyle_p& pStyle, float offset )
	{
		Label * pLabel = new Label(this);
		pLabel->textItem.set(text);
		pLabel->textItem.setStyle(pStyle);
		pLabel->offset = offset;
	    
		m_labels.pushBack(pLabel);
		_requestResize();
		_requestRender();
	}
	
	//____ setDirection() __________________________________________________________
	
	void RulerLabels::setDirection( Direction direction )
	{
		m_direction = direction;
		_requestResize();
		_requestRender();
	}
	
	//____ getLabel() ________________________________________________________________
	
	ModText_p	RulerLabels::getLabel(int index)
	{
		if( index >= m_labels.size() )
			return ModText_p();
	
		return ModText_p(this, &m_labels.get(index)->textInterface);
	}
	
	
	
	//____ preferredSize() ________________________________________________________________
	
	Size RulerLabels::preferredSize() const
	{
	    //TODO: calculation of length is not good.
	    
	    Size preferred;
	 
	    if( m_direction == Direction::Up || m_direction == Direction::Down )
	    {
			Label * pLabel = m_labels.first();
			while( pLabel )
	        {
				Size sz = pLabel->textItem.preferredSize();
	            if( sz.w > preferred.w )
	                preferred.w = sz.w;
	  
	            preferred.h += sz.h;
	            pLabel = pLabel->next();
	        }
	    }
	    else
	    {
			Label * pLabel = m_labels.first();
			while( pLabel )
	        {
	            Size sz = pLabel->textItem.preferredSize();
	            preferred.w += sz.w;
	            
	            if( sz.h > preferred.h )
	                preferred.h = sz.h;
	            
	            pLabel = pLabel->next();
	        }
	    }
	    
		if( m_pSkin )
			return m_pSkin->sizeForContent(preferred);
		else
			return preferred;
	}
	
	
	//____ _render() _____________________________________________________________________
	
	void RulerLabels::_render( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, const Rect& _clip )
	{
		Widget::_render(pDevice,_canvas,_window,_clip);
	
		Rect canvas;
		if( m_pSkin )
			canvas = m_pSkin->contentRect(_canvas,m_state);
		else
			canvas = _canvas;
	
		if( m_direction == Direction::Up || m_direction == Direction::Down )
		{
			Label * pLabel = m_labels.first();
			while( pLabel )
			{
				int height = pLabel->textItem.size().h;
				int ofs = (int) (canvas.h * pLabel->offset);
				if( m_direction == Direction::Up )
					ofs = canvas.h - ofs;
	/*
	TODO: Reinstate!!!
				
				switch( pLabel->textItem.alignment() )
				{
					case Origo::NorthWest:
					case Origo::North:
					case Origo::NorthEast:				
						break;
					case Origo::SouthEast:
					case Origo::South:
					case Origo::SouthWest:
						ofs -= height;
						break;
					case Origo::East:
					case Origo::West:
					case Origo::Center:
						ofs -= height/2;
						break;
				}
	*/			
				pLabel->textItem.onRender(pDevice, Rect( canvas.x, canvas.y + ofs, canvas.w, height ), _clip );
				pLabel = pLabel->next();
			}
		}
		else
		{
			Label * pLabel = m_labels.first();
			while( pLabel )
			{
				int width = pLabel->textItem.size().w;
				int ofs = (int) (canvas.w * pLabel->offset);
				if( m_direction == Direction::Left )
					ofs = canvas.w - ofs;
	/*			
	TODO: Reinstate!
	
				switch( pLabel->textItem.alignment() )
				{
					case Origo::NorthWest:
					case Origo::SouthWest:
					case Origo::West:
						break;
					case Origo::SouthEast:
					case Origo::NorthEast:				
					case Origo::East:
						ofs -= width;
						break;
					case Origo::North:
					case Origo::Center:
					case Origo::South:
						ofs -= width/2;
						break;
				}
	*/
				pLabel->textItem.onRender( pDevice, Rect( canvas.x + ofs, canvas.y, width, canvas.h ), _clip );
				pLabel = pLabel->next();
			}
		}
		
	}
	
	//____ _cloneContent() _________________________________________________________________ 
	
	void RulerLabels::_cloneContent( const Widget * _pOrg )
	{
		Widget::_cloneContent( _pOrg );
		
		const RulerLabels * pOrg = static_cast<const RulerLabels*>(_pOrg);
	}
	
	//____ _alphaTest() ____________________________________________________________________
	
	bool RulerLabels::_alphaTest( const Coord& ofs )
	{
		return Widget::_alphaTest(ofs);
	}
	
	
	//____ _setState() ______________________________________________________
	
	void RulerLabels::_setState( State state )
	{
		Widget::_setState(state);
	
		Label * p = m_labels.first();
		while( p )
		{
			p->textItem.setState(state);
			p = p->next();
		}
	}


	//____ _itemPos() __________________________________________________________

	Coord RulerLabels::_itemPos( const Item * pItem ) const
	{
		//TODO: Implement!!!
		return Coord();
	}
	
	//____ _itemSize() _________________________________________________________
	
	Size RulerLabels::_itemSize( const Item * pItem ) const
	{
		//TODO: Implement!!!
		return Size();
	}

	//____ _itemGeo() __________________________________________________________
	
	Rect RulerLabels::_itemGeo( const Item * pItem ) const
	{
		//TODO: Implement!!!
		return Rect();
	}
	
	//____ _resizeRequested() __________________________________________________

	void RulerLabels::_resizeRequested( const Item * pItem )
	{
		//TODO: Implement!!!
		_requestResize();
	}




} // namespace wg
