#include	<stdio.h>
#include	<wg_gizmo_profiler.h>
#include	<wg_pen.h>
#include	<wg_texttool.h>
#include	<wg_gfx.h>
#include	<wg_color.h>
#include	<wg_char.h>

#include "Utilities/EProfiler.h"

static const char	s_type[] = {"TordJ/Profiler"};


#define		TICK_BUFFER			64



//____ WgGizmoProfiler() _________________________________________________________

WgGizmoProfiler::WgGizmoProfiler( void )
{
}

//____ ~WgGizmoProfiler() __________________________________________________________

WgGizmoProfiler::~WgGizmoProfiler( void )
{

}


//____ Type() _________________________________________________________________

const char * WgGizmoProfiler::Type( void ) const
{
	return GetMyType();
}

const char * WgGizmoProfiler::GetMyType( void )
{
	return s_type;
}


//____ SetTextProperties() ____________________________________________________

void WgGizmoProfiler::SetTextProperties( WgTextPropPtr& pProp )
{
	m_pProp = pProp;
	RequestRender();
}



//____ OnRender() ________________________________________________________

void WgGizmoProfiler::OnRender( WgGfxDevice * pDevice, const WgRect& _window, const WgRect& _clip, Uint8 _layer )
{
	//____


	char	temp[400];
	WgChar temp2[400];
	char *	pTemp;
	pTemp = temp;

	WgPen pen( pDevice, _window, _clip );
	pen.SetTextProp( m_pProp );
	
	int height = pen.GetLineSpacing();

	EProfiler* pkProfiler = EProfiler::Get();
	
	Uint32 n = pkProfiler->GetTimerCount();
/*
	uint widgetHeight = (n+1)*height;
	if( widgetHeight < 80 )
		widgetHeight = 80;

	SetHeight( widgetHeight );
*/
	int a = 0x20;
	int b = 0x80;
	int c = 0xc0;

	WgColor aColors[6] = { 
		WgColor( c,	a, a ),
		WgColor( c,	c, a ),
		WgColor( a,	c, a ),
		WgColor( a,	c, c ),
		WgColor( a,	a, c ),
		WgColor( c,	a, a )
	};

	
	float fTimeScale = 1.0f; // Ms
/*	
	float fFrameTime = 1000*pkProfiler->GetFrameTime();
	while( fTimeScale < fFrameTime )
		fTimeScale *= 2.0f;

	static float fCurrentScale = 0.0f;
	static int iFrames = 0;
	
	if( fTimeScale >= fCurrentScale )
	{
		fCurrentScale = fTimeScale;
		iFrames = 0;
	}
	else
	{
		if( ++iFrames > 100 )
		{
			fCurrentScale = fTimeScale;
			iFrames = 0;
		}
	}

	fTimeScale = fCurrentScale;
*/

	fTimeScale = 20.0f;

	if( fTimeScale > 100 )
		fTimeScale = 100;


	// TODO: gradering.


	pDevice->ClipFill( _clip, ScreenGeometry(), WgColor(0x40,0x40,0x40,0x80) );
	


	for( Uint32 i=0; i<n; i++ )
	{
		float fTime = 10;
#ifdef BENCHMARK
		extern bool g_bF8;
		if( g_bF8 )
			fTime = 1000*pkProfiler->GetTimer(i)->GetTime();
		else
			fTime = 1000*pkProfiler->GetTimer(i)->GetLastAverage();
#endif

		float pixelsPerMs = Size().w/fTimeScale;
		Uint32 width = Uint32( fTime*pixelsPerMs );

		WgRect bar(_window.x, _window.y + height*i, width, height );

		pDevice->ClipFill( _clip, bar, aColors[i%6] );

		pTemp = temp;
		sprintf( pTemp, "%s: %.4f ms", pkProfiler->GetTimer(i)->GetName().GetChars(), fTime );
		WgTextTool::readString( (const char *&)pTemp, temp2, 39 );
		pDevice->PrintLine( &pen, m_pProp, WG_MODE_NORMAL, temp2 );
		pen.MoveY( height );
	}


	float fSpace = 10.0f;


	float fLine = fSpace;

	while( fLine < fTimeScale )
	{
		pTemp = temp;
		sprintf( pTemp, "%.4f ms", fLine );
		WgTextTool::readString( (const char *&)pTemp, temp2, 39 );
		int x = _window.x + int(Size().w*fLine/fTimeScale);
		pDevice->PrintLine( &pen, m_pProp, WG_MODE_NORMAL, temp2 );
		pen.MoveY( height );	
		fLine += fSpace;
	}

/*	pTemp = temp;
	sprintf( pTemp, "n00b" );
	WgTextTool::readString( pTemp, temp2, 39 );
	WgTextTool::TextParam param( * m_pFonts, WG_MODE_NORMAL, 3 );
	WgGfx::clipPrintLine( _clip, param, _window.x, _window.y, temp2 );

	pTemp = temp;
	sprintf( pTemp, "r374rd" );
	WgTextTool::readString( pTemp, temp2, 39 );
	WgGfx::clipPrintLine( _clip, param, _window.x, _window.y + height, temp2 );
*/
}

//____ OnUpdate() ________________________________________________________

void WgGizmoProfiler::OnUpdate( const WgUpdateInfo& _updateInfo )
{
	RequestRender();
}


//____ OnCloneContent() _______________________________________________________

void WgGizmoProfiler::OnCloneContent( const WgGizmo * _pOrg )
{
	WgGizmoProfiler * pOrg		= (WgGizmoProfiler *) _pOrg;

	m_pProp		= pOrg->m_pProp;
}


//____ OnMarkTest() ___________________________________________________

bool WgGizmoProfiler::OnMarkTest( const WgCord& ofs )
{
	return false;
}

