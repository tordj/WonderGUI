
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include <wg_surface.h>
#include <wg_surfacefactory.h>
#include <wg_resdb.h>

class WgEventHandler;
class Surface;
class SurfaceFactory;
class Font;

namespace sdl_wglib
{
	void 		MapKeys();
	void 		BeginEvents( const WgEventHandlerPtr& pHandler );
	void 		TranslateEvent( const WgEventHandlerPtr& pHandler, SDL_Event& event );
	void 		EndEvents( const WgEventHandlerPtr& pHandler );

	Surface_p loadSurface( const char * path, const SurfaceFactory_p& factory );

	ResDB_p	LoadStdWidgets( const char * pImagePath, const SurfaceFactory_p& factory );

	Font_p	LoadBitmapFont( const char * pImgPath, const char * pSpecPath, const SurfaceFactory_p& pFactory );
	
	int 		FileSize( const char * pPath );
	void * 		LoadFile( const char * pPath );
	
};
