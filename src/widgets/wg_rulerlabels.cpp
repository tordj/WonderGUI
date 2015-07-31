
#include <wg_rulerlabels.h>
#include <wg_gfxdevice.h>

namespace wg 
{
	
	const char WgRulerLabels::CLASSNAME[] = {"RulerLabels"};
	
	
	//____ Constructor ____________________________________________________________
	
	WgRulerLabels::WgRulerLabels()
	{
		m_direction = WG_RIGHT;
	}
	
	//____ Destructor _____________________________________________________________
	
	WgRulerLabels::~WgRulerLabels()
	{
	}
	
	//____ isInstanceOf() _________________________________________________________
	
	bool WgRulerLabels::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return WgWidget::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * WgRulerLabels::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	WgRulerLabels_p WgRulerLabels::cast( const WgObject_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return WgRulerLabels_p( static_cast<WgRulerLabels*>(pObject.rawPtr()) );
	
		return 0;
	}
	
	
	//____ AddLabel() ____________________________________________________________
	
	void WgRulerLabels::AddLabel( const WgCharSeq& text, const WgTextStyle_p& pStyle, float offset )
	{
		Label * pLabel = new Label(this);
		pLabel->textField.set(text);
		pLabel->textField.setStyle(pStyle);
		pLabel->offset = offset;
	    
		m_labels.pushBack(pLabel);
		_requestResize();
		_requestRender();
	}
	
	//____ SetDirection() __________________________________________________________
	
	void WgRulerLabels::SetDirection( WgDirection direction )
	{
		m_direction = direction;
		_requestResize();
		_requestRender();
	}
	
	//____ GetLabel() ________________________________________________________________
	
	WgModText_p	WgRulerLabels::GetLabel(int index)
	{
		if( index >= m_labels.size() )
			return WgModText_p();
	
		return WgModText_p(this, &m_labels.get(index)->textInterface);
	}
	
	
	
	//____ preferredSize() ________________________________________________________________
	
	WgSize WgRulerLabels::preferredSize() const
	{
	    //TODO: calculation of length is not good.
	    
	    WgSize preferred;
	 
	    if( m_direction == WG_UP || m_direction == WG_DOWN )
	    {
			Label * pLabel = m_labels.first();
			while( pLabel )
	        {
				WgSize sz = pLabel->textField.preferredSize();
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
	            WgSize sz = pLabel->textField.preferredSize();
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
	
	void WgRulerLabels::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip )
	{
		WgWidget::_onRender(pDevice,_canvas,_window,_clip);
	
		WgRect canvas;
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
				pLabel->textField.onRender(pDevice, WgRect( canvas.x, canvas.y + ofs, canvas.w, height ), _clip );
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
				pLabel->textField.onRender( pDevice, WgRect( canvas.x + ofs, canvas.y, width, canvas.h ), _clip );
				pLabel = pLabel->next();
			}
		}
		
	}
	
	//____ _onCloneContent() _________________________________________________________________ 
	
	void WgRulerLabels::_onCloneContent( const WgWidget * _pOrg )
	{
		const WgRulerLabels * pOrg = static_cast<const WgRulerLabels*>(_pOrg);
	}
	
	//____ _onAlphaTest() ____________________________________________________________________
	
	bool WgRulerLabels::_onAlphaTest( const WgCoord& ofs, const WgSize& sz )
	{
		return WgWidget::_onAlphaTest(ofs,sz);
	}
	
	
	//____ _onStateChanged() ______________________________________________________
	
	void WgRulerLabels::_onStateChanged( WgState oldState )
	{
		WgWidget::_onStateChanged(oldState);
	
		Label * p = m_labels.first();
		while( p )
		{
			p->textField.setState(m_state);
			p = p->next();
		}
	}
	
	//____ _onSkinChanged() _______________________________________________________
	
	void WgRulerLabels::_onSkinChanged( const WgSkin_p& pOldSkin, const WgSkin_p& pNewSkin )
	{
		WgWidget::_onSkinChanged(pOldSkin,pNewSkin);
	}
	
	//____ _onFieldDirty() _________________________________________________________
	
	void WgRulerLabels::_onFieldDirty( WgField * pField )
	{
		_requestRender();
	}
	
	//____ _onFieldResize() ________________________________________________________
	
	void WgRulerLabels::_onFieldResize( WgField * pField )
	{
		_requestResize();
		_requestRender();
	}

} // namespace wg
