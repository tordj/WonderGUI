
#include <wg_rulerlabels.h>
#include <wg_gfxdevice.h>

static const char	c_widgetType[] = {"RulerLabels"};


//____ Constructor ____________________________________________________________

WgRulerLabels::WgRulerLabels()
{
	m_direction = WG_RIGHT;
    m_pTextManager = 0;
}

//____ Destructor _____________________________________________________________

WgRulerLabels::~WgRulerLabels()
{
}

//____ Type() _________________________________________________________________

const char * WgRulerLabels::Type( void ) const
{
	return GetClass();
}

//____ GetClass() ____________________________________________________________

const char * WgRulerLabels::GetClass()
{
	return c_widgetType;
}

//____ AddLabel() ____________________________________________________________

void WgRulerLabels::AddLabel( const WgCharSeq& text, const WgTextpropPtr& pProps, float offset, WgOrigo origo )
{
	Label * pLabel = new Label();
	pLabel->text.setText(text);
	pLabel->text.setProperties(pProps);
	pLabel->text.setAlignment(origo);
    pLabel->text.setManager(m_pTextManager);
	pLabel->offset = offset;

    if( m_labels.IsEmpty() )
        pLabel->text.setHolder(this);
    
	m_labels.PushBack(pLabel);
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

//____ SetTextManager() ________________________________________________________

void WgRulerLabels::SetTextManager( WgTextManager * pTextManager )
{
    if( pTextManager != m_pTextManager )
    {
        m_pTextManager = pTextManager;
        
        Label * p = m_labels.First();
        while( p )
        {
            p->text.setManager(pTextManager);
            p = p->Next();
        }
    }

}


//____ PreferredSize() ________________________________________________________________

WgSize WgRulerLabels::PreferredSize() const
{
    //TODO: calculation of length is not good.
    
    WgSize preferred;
 
    if( m_direction == WG_UP || m_direction == WG_DOWN )
    {
		Label * pLabel = m_labels.First();
		while( pLabel )
        {
            int w = pLabel->text.unwrappedWidth();
            if( w > preferred.w )
                preferred.w = w;
  
            preferred.h += pLabel->text.getLine(0)->lineSpacing;            
            pLabel = pLabel->Next();
        }
    }
    else
    {
		Label * pLabel = m_labels.First();
		while( pLabel )
        {
            WgSize sz = pLabel->text.unwrappedSize();
            preferred.w += sz.w;
            
            if( sz.h > preferred.h )
                preferred.h = sz.h;
            
            pLabel = pLabel->Next();
        }
    }
    	
	return preferred;
}


//____ _onRender() _____________________________________________________________________

void WgRulerLabels::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip )
{
	if( m_direction == WG_UP || m_direction == WG_DOWN )
	{
		Label * pLabel = m_labels.First();
		while( pLabel )
		{
			int height = pLabel->text.height();
			int ofs = (int) (_canvas.h * pLabel->offset);
			if( m_direction == WG_UP )
				ofs = _canvas.h - ofs;
			
			switch( pLabel->text.alignment() )
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
			
			pDevice->PrintText( _clip, &pLabel->text, WgRect( _canvas.x, _canvas.y + ofs, _canvas.w, height ) );				
			pLabel = pLabel->Next();
		}
	}
	else
	{
		Label * pLabel = m_labels.First();
		while( pLabel )
		{
			int width = pLabel->text.width();
			int ofs = (int) (_canvas.w * pLabel->offset);
			if( m_direction == WG_LEFT )
				ofs = _canvas.w - ofs;
			
			switch( pLabel->text.alignment() )
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
			
			pDevice->PrintText( _clip, &pLabel->text, WgRect( _canvas.x + ofs, _canvas.y, width, _canvas.h ) );				
			pLabel = pLabel->Next();
		}
	}
	
}

//____ _onCloneContent() _________________________________________________________________ 

void WgRulerLabels::_onCloneContent( const WgWidget * _pOrg )
{
	const WgRulerLabels * pOrg = static_cast<const WgRulerLabels*>(_pOrg);
}

//____ _onAlphaTest() ____________________________________________________________________

bool WgRulerLabels::_onAlphaTest( const WgCoord& ofs )
{
	return false;
}

//____ _onTextModified() _________________________________________________________________

void WgRulerLabels::_textModified( WgText * pText )
{
    _requestResize();
    _requestRender();
}

