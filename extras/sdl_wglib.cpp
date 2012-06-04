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
		WgBase::MapKey( WG_KEY_PAGE_UP, SDLK_PAGEUP );
		WgBase::MapKey( WG_KEY_PAGE_DOWN, SDLK_PAGEDOWN );

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

		WgSurface * pSurf = factory.CreateSurface( wrapper.Size(), wrapper.IsOpaque()? WG_PIXEL_RGB_8 : WG_PIXEL_ARGB_8 );

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
		const int HSLIDER_BTN_OFS 		= 1;
		const int VSLIDER_BTN_OFS 		= HSLIDER_BTN_OFS + 19;
		const int SLIDER_OFS 			= VSLIDER_BTN_OFS + 19;
		const int SLIDER_BACK_OFS		= SLIDER_OFS + 10;
		const int RESIZE_BUTTON_OFS 	= SLIDER_BACK_OFS + 13;
		const int CHECKBOX_OFS 			= RESIZE_BUTTON_OFS + 22;
		const int RADIOBUTTON_OFS 		= CHECKBOX_OFS + 13;
		const int BUTTON_OFS			= RADIOBUTTON_OFS + 13;
		const int PLATE_OFS				= BUTTON_OFS + 10;
		const int SPLITS_AND_FRAME_OFS	= PLATE_OFS + 10;
		const int TILES_OFS				= 192;

		WgSurface * pSurface = LoadSurface( pImagePath, factory );
		if( !pSurface )
			return 0;

		WgBlockSetPtr pHSliderBtnBwdBlocks	= pSurface->defineBlockSet( WgHorrTile4( WgRect(1,HSLIDER_BTN_OFS,74,17), 2), WgBorders(3), WgBorders(4), 0, WG_OPAQUE );
		WgBlockSetPtr pHSliderBtnFwdBlocks	= pSurface->defineBlockSet( WgHorrTile4( WgRect(77,HSLIDER_BTN_OFS,74,17), 2), WgBorders(3), WgBorders(4), 0, WG_OPAQUE );

		WgBlockSetPtr pVSliderBtnBwdBlocks	= pSurface->defineBlockSet( WgHorrTile4( WgRect(1,VSLIDER_BTN_OFS,74,17), 2), WgBorders(3), WgBorders(4), 0, WG_OPAQUE );
		WgBlockSetPtr pVSliderBtnFwdBlocks	= pSurface->defineBlockSet( WgHorrTile4( WgRect(77,VSLIDER_BTN_OFS,74,17), 2), WgBorders(3), WgBorders(4), 0, WG_OPAQUE );

		WgBlockSetPtr pSliderBlocks			= pSurface->defineBlockSet( WgHorrTile4( WgRect(1,SLIDER_OFS,38,8), 2), WgBorders(2), WgBorders(3), 0, WG_OPAQUE );
		WgBlockSetPtr pSliderBackBlocks		= pSurface->defineBlockSet( WgRect(1,SLIDER_BACK_OFS,5,5), WgBorders(2), WgBorders(2), 0, WG_OPAQUE );

		WgBlockSetPtr pResizeButtonBlocks 	= pSurface->defineBlockSet( WgHorrTile4( WgRect(1,RESIZE_BUTTON_OFS,86,20), 2), WgBorders(3), WgBorders(0), 0, WG_OPAQUE );
		WgBlockSetPtr pCheckboxUncheckedBlocks	= pSurface->defineBlockSet( WgHorrTile4( WgRect(1,CHECKBOX_OFS,50,11), 2), WgBorders(2), WgBorders(3), 0, WG_OPAQUE );
		WgBlockSetPtr pCheckboxCheckedBlocks	= pSurface->defineBlockSet( WgHorrTile4( WgRect(53,CHECKBOX_OFS,50,11), 2), WgBorders(2), WgBorders(3), 0, WG_OPAQUE );
		WgBlockSetPtr pRadiobuttonUncheckedBlocks = pSurface->defineBlockSet( WgHorrTile4( WgRect(1,RADIOBUTTON_OFS,50,11), 2), WgBorders(0), WgBorders(3), 0, WG_OPAQUE );
		WgBlockSetPtr pRadiobuttonCheckedBlocks	= pSurface->defineBlockSet( WgHorrTile4( WgRect(53,RADIOBUTTON_OFS,50,11), 2), WgBorders(0), WgBorders(3), 0, WG_OPAQUE );

		WgBlockSetPtr pButtonBlocks 		= pSurface->defineBlockSet( WgHorrTile4( WgRect(1,BUTTON_OFS,38,8), 2), WgBorders(3), WgBorders(4), 0, WG_OPAQUE );
		WgBlockSetPtr pPlateBlocks 			= pSurface->defineBlockSet( WgHorrTile4( WgRect(1,PLATE_OFS,38,8), 2), WgBorders(3), WgBorders(4), 0, WG_OPAQUE );

		WgBlockSetPtr pHSplitBlocks			= pSurface->defineBlockSet( WgRect(1,SPLITS_AND_FRAME_OFS,8,2), WgBorders(0), WgBorders(0), 0, WG_OPAQUE );
		WgBlockSetPtr pVSplitBlocks			= pSurface->defineBlockSet( WgRect(11,SPLITS_AND_FRAME_OFS,2,8), WgBorders(0), WgBorders(0), 0, WG_OPAQUE );
		WgBlockSetPtr pFrameBlocks			= pSurface->defineBlockSet( WgRect(15,SPLITS_AND_FRAME_OFS,8,8), WgBorders(2), WgBorders(3), 0, 0 );

		WgBlockSetPtr pBgCheckeredGreyBlocks= pSurface->defineBlockSet( WgRect(0,TILES_OFS,64,64), WgBorders(0), WgBorders(0), 0, WG_OPAQUE | WG_TILE_ALL );
		WgBlockSetPtr pBgBlueGradientBlocks = pSurface->defineBlockSet( WgRect(1*64,TILES_OFS,64,64), WgBorders(0), WgBorders(0), 0, WG_OPAQUE );

		WgColorSetPtr pSelectionColors = WgColorSet::Create( WgColor(0x0), WgColor(0x40FFFFFF), WgColor(0x80FFFFFF), WgColor(0x40000000), WgColor(0x0) );


		WgResDB * pDB = new WgResDB();

		// Create standard button

		WgGizmoButton * pButton = new WgGizmoButton();
		pButton->SetSource( pButtonBlocks );
		pDB->AddGizmo( "button", pButton );

		// Create standard plate

		WgGizmoPixmap * pPlate = new WgGizmoPixmap();
		pPlate->SetSource( pPlateBlocks );
		pDB->AddGizmo( "plate", pPlate );

		// Create standard checkbox

		WgGizmoCheckbox * pCheckbox = new WgGizmoCheckbox();
		pCheckbox->SetIcons( pCheckboxUncheckedBlocks, pCheckboxCheckedBlocks );
		pDB->AddGizmo( "checkbox", pCheckbox );

		// Create standard radiobutton

		WgGizmoRadiobutton * pRadiobutton = new WgGizmoRadiobutton();
		pRadiobutton->SetIcons( pRadiobuttonUncheckedBlocks, pRadiobuttonCheckedBlocks );
		pDB->AddGizmo( "radiobutton", pRadiobutton );

		// Create standard horizontal slider

		WgGizmoHSlider * pHSlider = new WgGizmoHSlider();
		pHSlider->SetSource( pSliderBackBlocks, pSliderBlocks, pHSliderBtnBwdBlocks, pHSliderBtnFwdBlocks );
		pDB->AddGizmo( "hslider", pHSlider );

		// Create standard vertical slider

		WgGizmoVSlider * pVSlider = new WgGizmoVSlider();
		pVSlider->SetSource( pSliderBackBlocks, pSliderBlocks, pVSliderBtnBwdBlocks, pVSliderBtnFwdBlocks );
		pDB->AddGizmo( "vslider", pVSlider );


		// Create Background bitmaps

		WgGizmoPixmap * pBgCheckeredGrey = new WgGizmoPixmap();
		pBgCheckeredGrey->SetSource( pBgCheckeredGreyBlocks );
		pDB->AddGizmo( "bg_checkered_grey", pBgCheckeredGrey );

		WgGizmoPixmap * pBgBlueGradient = new WgGizmoPixmap();
		pBgBlueGradient->SetSource( pBgBlueGradientBlocks );
		pDB->AddGizmo( "bg_blue_gradient", pBgBlueGradient );

		// Create standard menu

		WgGizmoMenu * pMenu = new WgGizmoMenu();
		pMenu->SetBgSource( pPlateBlocks, 16, 16 );
		pMenu->SetSeparatorSource( pHSplitBlocks, WgBorders(1) );
		pMenu->SetCheckBoxSource( pCheckboxUncheckedBlocks, pCheckboxCheckedBlocks);
		pMenu->SetRadioButtonSource( pRadiobuttonUncheckedBlocks, pRadiobuttonCheckedBlocks);
		pMenu->SetSliderSource( pSliderBackBlocks, pSliderBlocks, pVSliderBtnBwdBlocks, pVSliderBtnFwdBlocks );
		pMenu->SetTileColors( pSelectionColors );
		pDB->AddGizmo( "menu", pMenu );

		// Create standard view
		
		{
			WgGizmoView * pView = new WgGizmoView();
			
			WgGizmo * pHSlider = pDB->CloneGizmo( "hslider" );
			WgGizmo * pVSlider = pDB->CloneGizmo( "vslider" );

			if( pHSlider )
				pView->SetHSlider( static_cast<WgGizmoHSlider*>(pHSlider) );
			if( pVSlider )
				pView->SetVSlider( static_cast<WgGizmoVSlider*>(pVSlider) );

			pView->SetFillerBlocks( pPlateBlocks );
			pDB->AddGizmo( "view", pView );
		}

		return pDB;
	}

	//____ LoadBitmapFont() ____________________________________________________

	WgFont * LoadBitmapFont( const char * pImgPath, const char * pSpecPath, const WgSurfaceFactory& factory )
	{
		//TODO: This leaks memory until we have ref-counted

		WgSurface * pFontImg = sdl_wglib::LoadSurface( pImgPath, factory );

		char * pFontSpec = (char*) LoadFile( pSpecPath );

		WgBitmapGlyphs * pGlyphs = new WgBitmapGlyphs( pFontImg, pFontSpec );

		WgFont * pFont = new WgFont();
		pFont->SetBitmapGlyphs( pGlyphs, WG_STYLE_NORMAL, 0 );

		free( pFontSpec );
		return pFont;
	}

	//____ FileSize() _____________________________________________________________

	int FileSize( const char * pPath )
	{
		FILE * fp = fopen( pPath, "rb" );
		if( !fp )
			return 0;

		fseek( fp, 0, SEEK_END );
		int size = ftell(fp);
		fseek( fp, 0, SEEK_SET );
		fclose( fp );

		return size;
	}

	//____ LoadFile() _____________________________________________________________

	void * LoadFile( const char * pPath )
	{
		FILE * fp = fopen( pPath, "rb" );
		if( !fp )
			return 0;

		fseek( fp, 0, SEEK_END );
		int size = ftell(fp);
		fseek( fp, 0, SEEK_SET );

		char * pMem = (char*) malloc( size+1 );
		pMem[size] = 0;
		int nRead = fread( pMem, 1, size, fp );
		fclose( fp );

		if( nRead < size )
		{
			free( pMem );
			return 0;
		}

		return pMem;

	}


};
