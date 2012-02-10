#include <sdl_wglib.h>
#include <wg_surface_sdl.h>
#include <wondergui.h>


namespace sdl_wglib
{
	static WgEventHandler * g_pHandler;
	static int				g_ticks = 0;

	//____ MapKeys() ___________________________________________________________

	void MapKeys()
	{
		WgBase::MapKey( WG_KEY_SHIFT, SDLK_LSHIFT );
		WgBase::MapKey( WG_KEY_SHIFT, SDLK_RSHIFT );
		WgBase::MapKey( WG_KEY_CONTROL, SDLK_LCTRL );
		WgBase::MapKey( WG_KEY_CONTROL, SDLK_RCTRL );
		WgBase::MapKey( WG_KEY_ALT, SDLK_LALT );
		WgBase::MapKey( WG_KEY_ALT, SDLK_RALT );

		WgBase::MapKey( WG_KEY_LEFT, SDLK_LEFT );
		WgBase::MapKey( WG_KEY_RIGHT, SDLK_RIGHT );
		WgBase::MapKey( WG_KEY_UP, SDLK_UP );
		WgBase::MapKey( WG_KEY_DOWN, SDLK_DOWN );

		WgBase::MapKey( WG_KEY_HOME, SDLK_HOME );
		WgBase::MapKey( WG_KEY_END, SDLK_END );
		WgBase::MapKey( WG_KEY_PAGEUP, SDLK_PAGEUP );
		WgBase::MapKey( WG_KEY_PAGEDOWN, SDLK_PAGEDOWN );

		WgBase::MapKey( WG_KEY_RETURN, SDLK_RETURN );
		WgBase::MapKey( WG_KEY_BACKSPACE, SDLK_BACKSPACE );
		WgBase::MapKey( WG_KEY_DELETE, SDLK_DELETE );
		WgBase::MapKey( WG_KEY_TAB, SDLK_TAB );
		WgBase::MapKey( WG_KEY_ESCAPE, SDLK_ESCAPE );
	}

	//____ BeginEvents() _______________________________________________________

	void BeginEvents( WgEventHandler * pHandler )
	{
		g_pHandler = pHandler;

		// Add a tick event first as the first.

		int ticks = SDL_GetTicks();
		pHandler->QueueEvent( new WgEvent::Tick( ticks - g_ticks ) );
		g_ticks = ticks;
	}

	//____ TranslateEvent() ____________________________________________________

	void TranslateEvent( SDL_Event& event )
	{
		switch (event.type)
		{
			// check for keypresses
			case SDL_KEYDOWN:
			{
				g_pHandler->QueueEvent( new WgEvent::KeyPress( event.key.keysym.sym ) );
				if( event.key.keysym.unicode != 0 )
					g_pHandler->QueueEvent( new WgEvent::Character( event.key.keysym.unicode ) );
				break;
			}

			case SDL_KEYUP:
			{
				g_pHandler->QueueEvent( new WgEvent::KeyRelease( event.key.keysym.sym ) );
				break;
			}

			case	SDL_MOUSEMOTION:
			{
				g_pHandler->QueueEvent( new WgEvent::MouseMove( WgCoord( event.motion.x, event.motion.y ) ) );
				break;
			}

			case	SDL_MOUSEBUTTONDOWN:
				if(event.button.button == 4 )
					g_pHandler->QueueEvent( new WgEvent::MouseWheelRoll( 1, 1 ) );
				else if(event.button.button == 5)
					g_pHandler->QueueEvent( new WgEvent::MouseWheelRoll( 1, -1 ) );
				else
				{
					g_pHandler->QueueEvent( new WgEvent::MouseButtonPress( event.button.button ) );
				}
				break;

			case	SDL_MOUSEBUTTONUP:
				if( event.button.button != 4 && event.button.button != 5 )
					g_pHandler->QueueEvent( new WgEvent::MouseButtonRelease( event.button.button ) );
				break;
		}
	}

	//_____ EndEvents() ________________________________________________________

	void EndEvents()
	{
		g_pHandler->ProcessEvents();
	}

	//____ LoadSurface() __________________________________________________________

	WgSurface * LoadSurface( const char * path, const WgSurfaceFactory& factory )
	{
		SDL_Surface* bmp = IMG_Load(path);
		if (!bmp)
		{
			printf("Unable to load bitmap: %s\n", IMG_GetError());
			return 0;
		}

		WgSurfaceSDL	wrapper( bmp );

		WgSurface * pSurf = factory.CreateSurface( wrapper.Size(), wrapper.IsOpaque()? WG_PIXEL_RGB_8 : WG_PIXEL_RGBA_8 );

		if( !pSurf )
		{
			printf("Unable to create surface for loaded bitmap: %s\n", path);
			return 0;
		}

		if( !pSurf->CopyFrom( &wrapper, WgCoord(0,0) ) )
		{
			printf("Unable to copy loaded bitmap '%s' to surface.\n", path);
			delete pSurf;
			return 0;
		};

		return pSurf;
	}

	//____ LoadStdGizmos() _____________________________________________________

	WgResDB * LoadStdGizmos( const char * pImagePath, const WgSurfaceFactory& factory )
	{
		const int BUTTON_OFS	= 110;
		const int BUTTON_OFS	= BUTTON_OFS + 10;
		const int TILES_OFS		= 192;

		WgSurface * pSurface = LoadSurface( pImagePath, factory );
		if( !pSurface )
			return 0;


		WgBlockSetPtr pButtonBlocks 		= pSurface->defineBlockSet( WgHorrTile4( WgRect(1,BUTTON_OFS,38,8), 2), WgBorders(3), WgBorders(4), 0, WG_OPAQUE );
		WgBlockSetPtr pPlateBlocks 			= pSurface->defineBlockSet( WgHorrTile4( WgRect(1,PLATE_OFS,38,8), 2), WgBorders(3), WgBorders(4), 0, WG_OPAQUE );

		WgBlockSetPtr pBgCheckeredGreyBlocks= pSurface->defineBlockSet( WgRect(0,TILES_OFS,64,64), WgBorders(0), WgBorders(0), 0, WG_OPAQUE );
		WgBlockSetPtr pBgBlueGradientBlocks = pSurface->defineBlockSet( WgRect(1*64,TILES_OFS,64,64), WgBorders(0), WgBorders(0), 0, WG_OPAQUE );


		WgResDB * pDB = new WgResDB();

		// Create standard button

		WgGizmoButton * pButton = new WgGizmoButton();
		pButton->SetSource( pButtonBlocks );
		pDB->AddGizmo( "button", pButton );

		// Create standard plate

		WgGizmoPixmap * pPlate = new WgGizmoPixmap();
		pPlate->SetSource( pPlateBlocks );
		pDB->AddGizmo( "plate", pPlate );


		// Create Background bitmaps

		WgGizmoPixmap * pBgCheckeredGrey = new WgGizmoPixmap();
		pBgCheckeredGrey->SetSource( pBgCheckeredGreyBlocks );
		pDB->AddGizmo( "bg_checkered_grey", pBgCheckeredGrey );

		WgGizmoPixmap * pBgBlueGradient = new WgGizmoPixmap();
		pBgBlueGradient->SetSource( pBgBlueGradientBlocks );
		pDB->AddGizmo( "bg_blue_gradient", pBgBlueGradient );

		return pDB;
	}



};
