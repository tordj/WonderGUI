
#ifdef WIN32
#include <SDL.h>
#include <SDL_image.h>
#else
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#endif

class WgEventHandler;
class WgSurface;
class WgSurfaceFactory;
class WgResDB;

namespace sdl_wglib
{
	void 		MapKeys();
	void 		BeginEvents( WgEventHandler * pHandler );
	void 		TranslateEvent( SDL_Event& event );
	void 		EndEvents();

	WgSurface * LoadSurface( const char * path, const WgSurfaceFactory& factory );

	WgResDB *	LoadStdGizmos( const char * pImagePath, const WgSurfaceFactory& factory );

};
