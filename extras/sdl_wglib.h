
#ifdef WIN32
#include <SDL.h>
#include <SDL_image.h>
#else
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#endif

class WgEventHandler;

namespace sdl_wglib
{
	void mapKeys();
	void beginEvents( WgEventHandler * pHandler );
	void translateEvent( SDL_Event& event );
	void endEvents();
};