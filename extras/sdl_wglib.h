
#ifdef WIN32
#include <SDL.h>
#include <SDL_image.h>
#else
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#endif

#ifndef WG_SURFACE_DOT_H
#	include <wg_surface.h>
#endif

#ifndef WG_SURFACEFACTORY_DOT_H
#	include <wg_surfacefactory.h>
#endif

class WgEventHandler;
class WgSurface;
class WgSurfaceFactory;
class WgResDB;
class WgFont;

namespace sdl_wglib
{
	void 		MapKeys();
	void 		BeginEvents( WgEventHandler * pHandler );
	void 		TranslateEvent( SDL_Event& event );
	void 		EndEvents();

	WgSurfacePtr LoadSurface( const char * path, const WgSurfaceFactoryPtr& factory );

	WgResDB *	LoadStdWidgets( const char * pImagePath, const WgSurfaceFactoryPtr& factory );

	WgFont *	LoadBitmapFont( const char * pImgPath, const char * pSpecPath, const WgSurfaceFactoryPtr& pFactory );
	
	int 		FileSize( const char * pPath );
	void * 		LoadFile( const char * pPath );
	
};
