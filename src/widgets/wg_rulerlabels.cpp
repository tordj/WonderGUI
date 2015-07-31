
#include <wg_rulerlabels.h>
#include <wg_gfxdevice.h>

namespace wg 
{
	
	const char RulerLabels::CLASSNAME[] = {"RulerLabels"};
	
	
	//____ Constructor ____________________________________________________________
	
	RulerLabels::RulerLabels()
	{
		m_direction = WG_RIGHT;
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
		pLabel->textField.set(text);
		pLabel->textField.setStyle(pStyle);
		pLabel->offset = offset;
	    
		m_labels.pushBack(pLabel);
		_requestResize();
		_requestRender();
	}
	
	//____ setDirection() __________________________________________________________
	
	void RulerLabels::setDirection( WgDirection direction )
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
	 
	    if( m_direction == WG_UP || m_direction == WG_DOWN )
	    {
			Label * pLabel = m_labels.first();
			while( pLabel )
	        {
				Size sz = pLabel->textField.preferredSize();
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
	            Size sz = pLabel->textField.preferredSize();
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
	
	
	//____ _onRender() _____________________________________________________________________
	
	void RulerLabels::_onRender( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, const Rect& _clip )
	{
		Widget::_onRender(pDevice,_canvas,_window,_clip);
	
		Rect canvas;
		if( m_pSkin )
			canvas = m_pSkin->contentRect(_canvas,m_state);
		else
			canvas = _canvas;
	
		if( m_direction == WG_UP || m_direction == WG_DOWN )
		{
			Label * pLabel = m_labels.first();
			while( pLabel )
			{
				int height = pLabel->textField.size().h;
				int ofs = (int) (canvas.h * pLabel->offset);
				if( m_direction == WG_UP )
					ofs = canvas.h - ofs;
	/*
	TODO: Reinstate!!!
				
				switch( pLabel->textField.alignment() )
				{
					case WG_NORTHWEST:
					case WG_NORTH:
					case WG_NORTHEAST:				
						break;
					case WG_SOUTHEAST:
					case WG_SOUTH:
					case WG_SOUTHWEST:
						ofs -= height;
						break;
					case WG_EAST:
					case WG_WEST:
					case WG_CENTER:
						ofs -= height/2;
						break;
				}
	*/			
				pLabel->textField.onRender(pDevice, Rect( canvas.x, canvas.y + ofs, canvas.w, height ), _clip );
				pLabel = pLabel->next();
			}
		}
		else
		{
			Label * pLabel = m_labels.first();
			while( pLabel )
			{
				int width = pLabel->textField.size().w;
				int ofs = (int) (canvas.w * pLabel->offset);
				if( m_direction == WG_LEFT )
					ofs = canvas.w - ofs;
	/*			
	TODO: Reinstate!
	
				switch( pLabel->textField.alignment() )
				{
					case WG_NORTHWEST:
					case WG_SOUTHWEST:
					case WG_WEST:
						break;
					case WG_SOUTHEAST:
					case WG_NORTHEAST:				
					case WG_EAST:
						ofs -= width;
						break;
					case WG_NORTH:
					case WG_CENTER:
					case WG_SOUTH:
						ofs -= width/2;
						break;
				}
	*/
				pLabel->textField.onRender( pDevice, Rect( canvas.x + ofs, canvas.y, width, canvas.h ), _clip );
				pLabel = pLabel->next();
			}
		}
		
	}
	
	//____ _onCloneContent() _________________________________________________________________ 
	
	void RulerLabels::_onCloneContent( const Widget * _pOrg )
	{
		const RulerLabels * pOrg = static_cast<const RulerLabels*>(_pOrg);
	}
	
	//____ _onAlphaTest() ____________________________________________________________________
	
	bool RulerLabels::_onAlphaTest( const Coord& ofs, const Size& sz )
	{
		return Widget::_onAlphaTest(ofs,sz);
	}
	
	
	//____ _onStateChanged() ______________________________________________________
	
	void RulerLabels::_onStateChanged( State oldState )
	{
		Widget::_onStateChanged(oldState);
	
		Label * p = m_labels.first();
		while( p )
		{
			p->textField.setState(m_state);
			p = p->next();
		}
	}
	
	//____ _onSkinChanged() _______________________________________________________
	
	void RulerLabels::_onSkinChanged( const Skin_p& pOldSkin, const Skin_p& pNewSkin )
	{
		Widget::_onSkinChanged(pOldSkin,pNewSkin);
	}
	
	//____ _onFieldDirty() _________________________________________________________
	
	void RulerLabels::_onFieldDirty( Field * pField )
	{
		_requestRender();
	}
	
	//____ _onFieldResize() ________________________________________________________
	
	void RulerLabels::_onFieldResize( Field * pField )
	{
		_requestResize();
		_requestRender();
	}

} // namespace wg
