#include <wg_textview.h>
#include <wg_textcontainer.h>


WgTextView::WgTextView()
{
	m_pContainer = 0;
	m_pNextView = 0;
}

WgTextView::~WgTextView()
{
	if( m_pContainer != 0 )
		m_pContainer->removeView( this );
}

void WgTextView::SetContainer( WgTextContainer * pContainer )
{
	if( m_pContainer != 0 )
		m_pContainer->removeView( this );

	if( pContainer == 0 )
	{
		m_pContainer = 0;
		m_pNextView = 0;
	}
	else
		pContainer->addView( this );

}	
