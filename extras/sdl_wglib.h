
#ifdef WIN32
#include <SDL.h>
#include <SDL_image.h>
#else
#       pragma clang diagnostic push
#       pragma clang diagnostic ignored "-Wdocumentation"
#       include <SDL2/SDL.h>
#       include <SDL2_image/SDL_image.h>
#       pragma clang diagnostic pop
//#include <SDL2/SDL_image.h>
#endif

#include <wg_types.h>

#include <wg3_surface.h>

namespace wg
{
    class SurfaceFactory;
};


class WgEventHandler;
class WgResDB;
class WgFont;

namespace sdl_wglib
{
	void 		MapKeys();
	void 		BeginEvents( WgEventHandler * pHandler );
	void 		TranslateEvent( SDL_Event& event );
	void 		EndEvents();

    wg::Surface_p LoadSurface( const char * path, const wg::SurfaceFactory * factory );
    bool		SavePNG( wg::Surface * pSurface, const char * path);

    WgResDB *	LoadStdWidgets( const char * pImagePath, const char * pImagePathX2, const char * pImagePathX4, const wg::SurfaceFactory * factory );

    WgFont *	LoadBitmapFont( const char * pImgPath, const char * pSpecPath, const wg::SurfaceFactory * factory );
	
	int 		FileSize( const char * pPath );
	void * 		LoadFile( const char * pPath );

	void		ConvertPixelFormat( WgPixelFormat * pWGFormat, const SDL_PixelFormat * pSDLFormat );
	
};
