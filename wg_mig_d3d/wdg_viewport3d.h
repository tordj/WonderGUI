#ifndef	WDG_VIEWPORT3D_DOT_H
#define	WDG_VIEWPORT3D_DOT_H


#include "EObjects\\ECamera.h"
#include "EObjects/ESGObject.h"

#ifndef	WG_GIZMO_WIDGET_WRAPPER_DOT_H
#	include <wg_gizmo_widget_wrapper.h>
#endif


#ifndef WG_GIZMO_VIEWPORT3D_DOT_H
#	include <wg_gizmo_viewport3d.h>
#endif

class Wdg_Viewport3D : public Wdg_Widget<WgGizmoViewport3D>
{
public:
	Wdg_Viewport3D( void ) {};
	Wdg_Viewport3D ( const WgRect& geometry, WgWidget * pParent = 0 ) : Wdg_Widget<WgGizmoViewport3D>(geometry, pParent) {};
	Wdg_Viewport3D( WgOrigo origo, const WgRect& geometry, WgWidget * pParent = 0 ) : Wdg_Widget<WgGizmoViewport3D>( origo, geometry, pParent ) {};
	Wdg_Viewport3D( WgOrigo upperLeft, Sint32 x1, Sint32 y1, WgOrigo lowerRight, Sint32 x2, Sint32 y2, WgWidget * pParent = 0 ) : Wdg_Widget<WgGizmoViewport3D>(upperLeft,x1, y1, lowerRight, x2, y2, pParent) {}	
	virtual ~Wdg_Viewport3D() {};
};

/*
#ifndef	WG_WIDGET_DOT_H
#	include <wg_widget.h>
#endif

class Wdg_Viewport3D:public WgWidget
{
	public:
		WIDGET_CONSTRUCTORS(Wdg_Viewport3D,WgWidget)
		~Wdg_Viewport3D();

		virtual const char * Type() const;
		static const char * GetMyType();

		void	SetCamera( const ECameraPtr& spCamera );
		inline const ECameraPtr& GetCamera() { return m_spCamera; };

		void	SetScene( const ESGObjectPtr& spScene );
		inline const ESGObjectPtr& GetScene() { return m_spScene; };

		EVector3 WorldToScreen( const EVector3& kWorld, ECameraPtr spCamera = NULL );

	protected:
		WgWidget * NewOfMyType() const { return new Wdg_Viewport3D; };
	private:
		void	Init();
		void	DoMyOwnRender( const WgRect& window, const WgRect& clip, Uint8 layer );	
		void	DoMyOwnRefresh( void );
		void	DoMyOwnUpdate( const WgUpdateInfo& _updateInfo );
		void	DoMyOwnCloning( WgWidget * _pClone, const WgWidget * _pCloneRoot, const WgWidget * _pBranchRoot );
		bool 	DoMyOwnMarkTest( int _x, int _y );
		void 	DoMyOwnDisOrEnable( void );
	

		ECameraPtr		m_spCamera;  
		ESGObjectPtr	m_spScene;

};
*/

#endif //	WDG_VIEWPORT3D_DOT_H
