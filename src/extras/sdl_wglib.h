
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#ifndef WG_SURFACE_DOT_H
#	include <wg_surface.h>
#endif

#ifndef WG_SURFACEFACTORY_DOT_H
#	include <wg_surfacefactory.h>
#endif

#ifndef WG_RESDB_DOT_H
#	include <wg_resdb.h>
#endif

class WgEventHandler;
class WgSurface;
class WgSurfaceFactory;
class WgFont;

namespace sdl_wglib
{
	void 		MapKeys();
	void 		BeginEvents( const WgEventHandlerPtr& pHandler );
	void 		TranslateEvent( const WgEventHandlerPtr& pHandler, SDL_Event& event );
	void 		EndEvents( const WgEventHandlerPtr& pHandler );

	WgSurface_p loadSurface( const char * path, const WgSurfaceFactory_p& factory );

	WgResDB_p	LoadStdWidgets( const char * pImagePath, const WgSurfaceFactory_p& factory );

	WgFont_p	LoadBitmapFont( const char * pImgPath, const char * pSpecPath, const WgSurfaceFactory_p& pFactory );
	
	int 		FileSize( const char * pPath );
	void * 		LoadFile( const char * pPath );
	
};
