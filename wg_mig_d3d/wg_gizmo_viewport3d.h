#ifndef	WG_GIZMO_VIEWPORT3D_DOT_H
#define	WG_GIZMO_VIEWPORT3D_DOT_H


#include "EObjects\\ECamera.h"
#include "EObjects/ESGObject.h"


#ifndef	WG_GIZMO_DOT_H
#	include <wg_gizmo.h>
#endif

class WgGizmoViewport3D:public WgGizmo
{
	public:
		WgGizmoViewport3D();
		~WgGizmoViewport3D();

		virtual const char * Type() const;
		static const char * GetMyType();

		void	SetCamera( const ECameraPtr& spCamera );
		inline const ECameraPtr& GetCamera() { return m_spCamera; };

		void	SetScene( const ESGObjectPtr& spScene );
		inline const ESGObjectPtr& GetScene() { return m_spScene; };

		EVector3 WorldToScreen( const EVector3& kWorld, ECameraPtr spCamera = NULL );

	protected:
		void	OnCloneContent( const WgGizmo * _pOrg );
		void	OnRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip, Uint8 _layer );
		void	OnRefresh();
		void	OnUpdate( const WgUpdateInfo& _updateInfo );
		bool	OnMarkTest( const WgCord& ofs );

	private:
	

		ECameraPtr		m_spCamera;  
		ESGObjectPtr	m_spScene;

};


#endif //	WG_GIZMO_VIEWPORT3D_DOT_H
