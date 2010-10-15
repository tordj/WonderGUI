
#include <wg_gizmo_viewport3d.h>

#include "EObjects/ERenderer.h"
//#include "../../Intobet/Misc/Util/Util.hpp"

static const char	s_type[] = {"TordJ/Viewport3D"};

//____ WgGizmoViewport3D() _________________________________________________________________

WgGizmoViewport3D::WgGizmoViewport3D( void )
{
	m_bRendersAll	= true;
}

//____ ~WgGizmoViewport3D() ______________________________________________________

WgGizmoViewport3D::~WgGizmoViewport3D( void )
{
}


//____ Type() _________________________________________________________________

const char * WgGizmoViewport3D::Type( void ) const
{
	return GetMyType();
}

const char * WgGizmoViewport3D::GetMyType( void )
{
	return s_type;
}


//____ SetCamera() ____________________________________________________________

void WgGizmoViewport3D::SetCamera( const ECameraPtr& spCamera )
{
	m_spCamera = spCamera;
}

//____ SetScene() _____________________________________________________________

void WgGizmoViewport3D::SetScene( const ESGObjectPtr& spScene )
{
	m_spScene = spScene;
}

//____ OnRefresh() _______________________________________________________

void WgGizmoViewport3D::OnRefresh( void )
{
    RequestRender();
}

//____ OnUpdate() ________________________________________________________

void	WgGizmoViewport3D::OnUpdate( const WgUpdateInfo& _updateInfo )
{
	RequestRender();
}


//____ OnRender() ________________________________________________________

#ifdef BENCHMARK
extern bool g_bF4;
#endif

void WgGizmoViewport3D::OnRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& window, const WgRect& _clip, Uint8 _layer )
{
	if( !m_bEnabled || m_spCamera == NULL || m_spScene == NULL )
		return;

//	assert( _canvas.x == _clip.x && _canvas.y == _clip.y && _canvas.w == _clip.w && _canvas.h == _clip.h );
	if( _canvas.x != _clip.x && _canvas.y != _clip.y && _canvas.w != _clip.w && _canvas.h != _clip.h )
		return;

	uint w = ERenderer::GetCurrent()->GetCurrentWidth();
	uint h = ERenderer::GetCurrent()->GetCurrentHeight();

#ifdef BENCHMARK
	if( g_bF4 ) {
		ERenderer::GetCurrent()->SetViewPort( _canvas.x/4, _canvas.y/4, _canvas.w/4, _canvas.h/4 );
	} else
#endif
	{
		ERenderer::GetCurrent()->SetViewPort( _canvas.x, _canvas.y, _canvas.w, _canvas.h );
	}
	// Clear Z-buffer if this viewport has the standard 3D-layer
	if( (_layer & 1) != 0 )
		ERenderer::GetCurrent()->Clear(false, true);

	// Clear Z-buffer if this viewport has the standard WonderGUI GUI layer
	if( (_layer & 128) != 0 )
		ERenderer::GetCurrent()->Clear(false, true);

	m_spCamera->AdjustAspectRatio( float(_canvas.w), float(_canvas.h) );
	m_spCamera->Render( m_spScene, _layer );

	ERenderer::GetCurrent()->SetViewPort( 0, 0, w, h );
}

//____ OnCloneContent() _______________________________________________________

void WgGizmoViewport3D::OnCloneContent( const WgGizmo * _pOrg )
{
	WgGizmoViewport3D * pOrg = (WgGizmoViewport3D *) _pOrg;

	m_spCamera		= pOrg->m_spCamera;
	m_spScene		= pOrg->m_spScene;
}

//____ OnAlphaTest() ______________________________________________________

bool WgGizmoViewport3D::OnAlphaTest( const WgCord& ofs )
{
	return true;
}


/*
//____ Pick3DObject() _________________________________________________________

WgGizmoViewport3D::Pick3DObject( Sint32 x, Sint32 y )
{

	ERenderer::GetCurrent()->ScreenToWorld( x, y, kRayPosition, kRayDirection );



		//////////////////////////////////////////////////////////////////////////
		// the following raypicking code is not yet implemented in the other game tables
		//////////////////////////////////////////////////////////////////////////
		{ // BEGIN BLOCK-COMMMENT
		
			/* get the 3d-projected 2d mouse coords */
		//	POINT p; GetCursorPos( &p );
		//	EVector kRayPosition, kRayDirection;
		//	ERenderer::GetCurrent()->ScreenToWorld( p.x, p.y, kRayPosition, kRayDirection );

			/* get the current cameras z-rotation */
		//	EMatrix3 kCameraRot = m_spCamera->GetWorldTransform().GetLinear();

			/* adjust the kCameraRot.Z according to our kRayDirection vector here */
			/* however this is not accurate yet due to viewport scaling */

			/* the RayPick function will return an EArray of EMesh* with the FIRST intersected object in the beginning */
			/* of the vector - so kIntersectedMeshes[0] is the first mesh that was intersected*/
		//	EArray < EMesh* > kIntersectedMeshes = ESGUtil::RayPick( kRayPosition, kCameraRot.Z, 100000 );
		
			
//			for( unsigned int i = 0; i < kIntersectedMeshes.GetSize(); ++i )

		/* adjust the kCameraRot.Z according to our kRayDirection vector here */
		/* however this is not accurate yet due to viewport scaling */

		/* the RayPick function will return an EArray of EMesh* with the FIRST intersected object in the beginning */
		/* of the vector - so kIntersectedMeshes[0] is the first mesh that was intersected*/
		
		/*EArray < EMesh* > kIntersectedMeshes = ESGUtil::RayPick( kRayPosition, kCameraRot.Z, 100000 );
										
		for( int i = 0; i < kIntersectedMeshes.GetSize(); ++i )
		{
			if( GetPlayerInstance() )
			{
				if( GetPlayerInstance() )
				{
					// there is a possibility that a node is unnamed
					if( kIntersectedMeshes[0]->GetName().GetChars() )
						GetPlayerInstance()->AddLogText( "[%i] %s", 0, kIntersectedMeshes[0]->GetName().GetChars() );	
					else // so get the node parent name instead
						GetPlayerInstance()->AddLogText( "[%i] %s", 0, kIntersectedMeshes[0]->GetParent()->GetName().GetChars() );	
				}
				/* there is a possibility that a node is unnamed *//*
				if( kIntersectedMeshes[0]->GetName().GetChars() )
					GetPlayerInstance()->AddLogText( "[%i] %s", 0, kIntersectedMeshes[0]->GetName().GetChars() );	
				else // so get the node parent name instead
					GetPlayerInstance()->AddLogText( "[%i] %s", 0, kIntersectedMeshes[0]->GetParent()->GetName().GetChars() );	

			}
			
		} // END BLOCK COMMENT

		//////////////////////////////////////////////////////////////////////////
		// [END] the following raypicking code is not yet implemented in the other game tables
		//////////////////////////////////////////////////////////////////////////


}
*/

//____ WorldToScreen() ________________________________________________________

EVector3 WgGizmoViewport3D::WorldToScreen( const EVector3& kWorld, ECameraPtr spCamera )
{
/*
	uint w = ERenderer::GetCurrent()->GetWidth();
	uint h = ERenderer::GetCurrent()->GetHeight();

	ERenderer::GetCurrent()->SetViewPort( 0, 0, m_geo.w, m_geo.h );

	m_spCamera->SetAspectRatio( (float) m_geo.w, (float) m_geo.h );

	ERenderer::GetCurrent()->SetCamera( m_spCamera );
	EVector3 kScreen = ERenderer::GetCurrent()->WorldToScreen( kWorld );

	ERenderer::GetCurrent()->SetViewPort( 0, 0, w, h );
*/
	static EVector3 kZero( 0.f, 0.f, 0.f );
	if( NULL == m_pHook )
		return kZero;


	ERenderer* pRenderer = ERenderer::GetCurrent();


	const ECameraPtr& spOldCamera = pRenderer->GetCamera();

	if( !spCamera )
		spCamera = m_spCamera;

	if( NULL == spCamera )
		return kZero;

//	uint w = ERenderer::GetCurrent()->GetWidth();
//	uint h = ERenderer::GetCurrent()->GetHeight();

//	if( m_geo.w > (int)w || m_geo.h > (int)h )
//		return kZero;

	// shouldn't x/y be the widget's absolute x/y or something ?
//	pRenderer->SetViewPort( 0, 0, m_geo.w, m_geo.h );

	WgSize sz = Size();

	spCamera->AdjustAspectRatio(float(sz.w), float(sz.h));

	pRenderer->SetCamera( spCamera );
	EVector3 kScreen = pRenderer->WorldToScreen( kWorld );

	kScreen.x *= (float)sz.w;
	kScreen.y *= (float)sz.h;


// fulhack bort	pRenderer->SetViewPort( 0, 0, w, h );

//	kScreen.x *= ((float)m_geo.w)/w;
//	kScreen.y *= ((float)m_geo.h)/h;

	// restore previous camera
	pRenderer->SetCamera( spOldCamera );


	return kScreen;
}
