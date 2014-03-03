#include <sdl_wglib.h>
#include <wg_sdlsurface.h>
#include <wondergui.h>


namespace sdl_wglib
{
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

	void BeginEvents( const WgEventHandlerPtr& pHandler )
	{
		// Add a tick event first as the first.

		int ticks = SDL_GetTicks();
		pHandler->QueueEvent( WgTickEvent::Create( ticks - g_ticks ) );
		g_ticks = ticks;
	}

	//____ TranslateEvent() ____________________________________________________

	void TranslateEvent(  const WgEventHandlerPtr& pHandler, SDL_Event& event )
	{
		switch (event.type)
		{
			// check for keypresses
			case SDL_KEYDOWN:
			{
				pHandler->QueueEvent( WgKeyPressEvent::Create( event.key.keysym.sym ) );
				if( event.key.keysym.unicode != 0 )
					pHandler->QueueEvent( WgCharacterEvent::Create( event.key.keysym.unicode ) );
				break;
			}

			case SDL_KEYUP:
			{
				pHandler->QueueEvent( WgKeyReleaseEvent::Create( event.key.keysym.sym ) );
				break;
			}

			case	SDL_MOUSEMOTION:
			{
				pHandler->QueueEvent( WgMouseMoveEvent::Create( WgCoord( event.motion.x, event.motion.y ) ) );
				break;
			}

			case	SDL_MOUSEBUTTONDOWN:
				if(event.button.button == 4 )
					pHandler->QueueEvent( WgWheelRollEvent::Create( 1, 1 ) );
				else if(event.button.button == 5)
					pHandler->QueueEvent( WgWheelRollEvent::Create( 1, -1 ) );
				else
				{
					pHandler->QueueEvent( WgMousePressEvent::Create( event.button.button ) );
				}
				break;

			case	SDL_MOUSEBUTTONUP:
				if( event.button.button != 4 && event.button.button != 5 )
					pHandler->QueueEvent( WgMouseReleaseEvent::Create( event.button.button ) );
				break;
		}
	}

	//_____ EndEvents() ________________________________________________________

	void EndEvents( const WgEventHandlerPtr& pHandler )
	{
		pHandler->ProcessEvents();
	}

	//____ LoadSurface() __________________________________________________________

	WgSurfacePtr LoadSurface( const char * path, const WgSurfaceFactoryPtr& pFactory )
	{
		SDL_Surface* bmp = IMG_Load(path);
		if (!bmp)
		{
			printf("Unable to load bitmap: %s\n", IMG_GetError());
			return 0;
		}

		WgSDLSurfacePtr	wrapper = WgSDLSurface::Create( bmp );

		WgSurfacePtr pSurf = pFactory->CreateSurface( wrapper->Size(), wrapper->IsOpaque()? WG_PIXEL_RGB_8 : WG_PIXEL_ARGB_8 );

		if( !pSurf )
		{
			printf("Unable to create surface for loaded bitmap: %s\n", path);
			return 0;
		}

		if( !pSurf->CopyFrom( wrapper, WgCoord(0,0) ) )
		{
			printf("Unable to copy loaded bitmap '%s' to surface.\n", path);
			return 0;
		};

		return pSurf;
	}

	//____ LoadStdWidgets() _____________________________________________________

	WgResDBPtr LoadStdWidgets( const char * pImagePath, const WgSurfaceFactoryPtr& pFactory )
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
		const int COMBOBOX_OFS			= SPLITS_AND_FRAME_OFS + 10;
		const int TILES_OFS				= 192;

		WgSurfacePtr pSurface = LoadSurface( pImagePath, pFactory );
		if( !pSurface )
			return 0;

		WgBlockSkinPtr	pHScrollbarBwdButtonSkin = WgBlockSkin::CreateClickable( pSurface, WgSize(17,17), WgCoord(1,HSLIDER_BTN_OFS), WgSize(19,0), WgBorders(3) );
		pHScrollbarBwdButtonSkin->SetContentPadding(WgBorders(4));

		WgBlockSkinPtr	pHScrollbarFwdButtonSkin = WgBlockSkin::CreateClickable( pSurface, WgSize(17,17), WgCoord(77,HSLIDER_BTN_OFS), WgSize(19,0), WgBorders(3) );
		pHScrollbarFwdButtonSkin->SetContentPadding(WgBorders(4));

		WgBlockSkinPtr	pVScrollbarBwdButtonSkin = WgBlockSkin::CreateClickable( pSurface, WgSize(17,17), WgCoord(1,VSLIDER_BTN_OFS), WgSize(19,0), WgBorders(3) );
		pVScrollbarBwdButtonSkin->SetContentPadding(WgBorders(4));

		WgBlockSkinPtr	pVScrollbarFwdButtonSkin = WgBlockSkin::CreateClickable( pSurface, WgSize(17,17), WgCoord(77,VSLIDER_BTN_OFS), WgSize(19,0), WgBorders(3) );
		pVScrollbarFwdButtonSkin->SetContentPadding(WgBorders(4));

		WgBlockSkinPtr	pScrollbarHandleSkin = WgBlockSkin::CreateClickable( pSurface, WgSize(8,8), WgCoord(1,SLIDER_OFS), WgSize(10,0), WgBorders(2) );
		pScrollbarHandleSkin->SetContentPadding(WgBorders(3));

		WgBlockSkinPtr	pScrollbarBackSkin = WgBlockSkin::CreateStatic( pSurface, WgRect(1,SLIDER_BACK_OFS,5,5), WgBorders(2) );
		pScrollbarBackSkin->SetContentPadding(WgBorders(2));

//		WgBlocksetPtr pResizeButtonBlocks = WgBlockset::CreateFromRow(pSurface, WgRect(1,RESIZE_BUTTON_OFS,86,20), 4, 2, WG_OPAQUE);
//		pResizeButtonBlocks->SetFrame(WgBorders(3));

		WgBlockSkinPtr	pCheckboxIconSkin = WgBlockSkin::CreateClickSelectableWidget( pSurface, WgSize(11,11), WgCoord(1,CHECKBOX_OFS), WgSize(13,0), WgBorders(2) );
		pCheckboxIconSkin->SetContentPadding(WgBorders(3));

		WgBlockSkinPtr	pRadiobuttonIconSkin = WgBlockSkin::CreateClickSelectableWidget( pSurface, WgSize(11,11), WgCoord(1,RADIOBUTTON_OFS), WgSize(13,0) );
		pRadiobuttonIconSkin->SetContentPadding(WgBorders(3));

		WgBlockSkinPtr pButtonSkin = WgBlockSkin::CreateClickable( pSurface, WgSize(8,8), WgCoord(1,BUTTON_OFS), WgSize(10,0), WgBorders(3) );
		pButtonSkin->SetContentPadding(WgBorders(4));
		pButtonSkin->SetContentShift( WG_STATE_PRESSED, WgCoord(2,2) );

		WgBlockSkinPtr pHSplitSkin = WgBlockSkin::CreateStatic(pSurface, WgRect(1,SPLITS_AND_FRAME_OFS,8,2), WgBorders(0) );

		WgBlockSkinPtr pVSplitBlocks = WgBlockSkin::CreateStatic(pSurface, WgRect(11,SPLITS_AND_FRAME_OFS,2,8), WgBorders(0) );

		WgBlockSkinPtr pFrameSkin = WgBlockSkin::CreateClickable( pSurface, WgSize(8,8), WgCoord(1, SPLITS_AND_FRAME_OFS), WgSize(10,0), WgBorders(2) );
		pFrameSkin->SetContentPadding(WgBorders(3));
		
//		WgBlocksetPtr pComboboxBlocks = WgBlockset::CreateFromRow(pSurface, WgRect(1,COMBOBOX_OFS,98,20), 4, 2, WG_OPAQUE);
//		pComboboxBlocks->SetFrame(WgBorders(1,1,20,1));
//		pComboboxBlocks->SetPadding(WgBorders(2,2,21,2));

		WgBlockSkinPtr pBgCheckeredGreySkin = WgBlockSkin::CreateStatic( pSurface, WgRect(0,TILES_OFS,64,64) );
		pBgCheckeredGreySkin->SetTiledCenter(true);
		pBgCheckeredGreySkin->OptimizeRenderMethods();

		WgBlockSkinPtr pBgBlueGradientSkin = WgBlockSkin::CreateStatic( pSurface, WgRect(1*64,TILES_OFS,64,64) );
		pBgBlueGradientSkin->OptimizeRenderMethods();


//		WgColorsetPtr pSelectionColors = WgColorset::Create( WgColor(0x0), WgColor(0x40FFFFFF), WgColor(0x80FFFFFF), WgColor(0x40000000), WgColor(0x0) );

		WgBlockSkinPtr pPlateSkin = WgBlockSkin::CreateClickable( pSurface, WgSize(8,8), WgCoord(1,PLATE_OFS), WgSize(10,0), WgBorders(3) );
		pPlateSkin->SetContentPadding(WgBorders(4));
		pPlateSkin->OptimizeRenderMethods();

		WgResDBPtr pDB = WgResDB::Create();

		// Create standard button

		WgButtonPtr pButton = WgButton::Create();
		pButton->SetSkin( pButtonSkin );
		pDB->AddWidget( "button", pButton );

		// Create standard plate

		WgImagePtr pPlate = WgImage::Create();
		pPlate->SetSkin( pPlateSkin );
		pDB->AddWidget( "plate", pPlate );

		// Create standard checkbox

		WgCheckBoxPtr pCheckbox = WgCheckBox::Create();
		pCheckbox->SetIcon( pCheckboxIconSkin );
		pDB->AddWidget( "checkbox", pCheckbox );

		// Create standard radiobutton

		WgRadioButtonPtr pRadiobutton = WgRadioButton::Create();
		pRadiobutton->SetIcon( pRadiobuttonIconSkin );
		pDB->AddWidget( "radiobutton", pRadiobutton );

		// Create standard horizontal scrollbar

		WgHScrollbarPtr pHScrollbar = WgHScrollbar::Create();
		pHScrollbar->SetSkins( pScrollbarBackSkin, pScrollbarHandleSkin, pHScrollbarBwdButtonSkin, pHScrollbarFwdButtonSkin );
		pDB->AddWidget( "hscrollbar", pHScrollbar );

		// Create standard vertical scrollbar

		WgVScrollbarPtr pVScrollbar = WgVScrollbar::Create();
		pVScrollbar->SetSkins( pScrollbarBackSkin, pScrollbarHandleSkin, pVScrollbarBwdButtonSkin, pVScrollbarFwdButtonSkin );
		pDB->AddWidget( "vscrollbar", pVScrollbar );

		// Create standard menubar
/*
		WgMenubar * pMenubar = new WgMenubar();
		pMenubar->SetBgSource( pPlateBlocks );
		pDB->AddWidget( "menubar", pMenubar );
*/
		// Create Background bitmaps

		WgImagePtr pBgCheckeredGrey = WgImage::Create();
		pBgCheckeredGrey->SetSkin( pBgCheckeredGreySkin );
		pDB->AddWidget( "bg_checkered_grey", pBgCheckeredGrey );

		WgImagePtr pBgBlueGradient = WgImage::Create();
		pBgBlueGradient->SetSkin( pBgBlueGradientSkin );
		pDB->AddWidget( "bg_blue_gradient", pBgBlueGradient );

		// Create standard menu

		WgMenuPtr pMenu = WgMenu::Create();
		pMenu->SetSkin( pPlateSkin, 16, 16 );
		pMenu->SetSeparatorSkin( pHSplitSkin, WgBorders(1) );
		pMenu->SetCheckBoxSkin( pCheckboxIconSkin );
		pMenu->SetRadioButtonSkin( pRadiobuttonIconSkin );
		pMenu->SetScrollbarSkins( pScrollbarBackSkin, pScrollbarHandleSkin, pVScrollbarBwdButtonSkin, pVScrollbarFwdButtonSkin );
		pMenu->SetEntrySkin( pPlateSkin );
		pDB->AddWidget( "menu", pMenu );

		// Create standard combobox
/*
		WgCombobox * pCombobox = new WgCombobox();
		pCombobox->SetSource( pComboboxBlocks );
		pDB->AddWidget( "combobox", pCombobox );
*/
		// Create standard view
		
		{
			WgScrollPanelPtr pView = WgScrollPanel::Create();
			
			WgWidgetPtr pHScrollbar = pDB->CloneWidget( "hscrollbar" );
			WgWidgetPtr pVScrollbar = pDB->CloneWidget( "vscrollbar" );

			if( pHScrollbar )
			{
				WgHScrollbarPtr pH = WgHScrollbar::Cast(pHScrollbar);
				pView->SetHScrollbar( pH );
			}
			if( pVScrollbar )
			{
				WgVScrollbarPtr pV = WgVScrollbar::Cast(pVScrollbar);
				pView->SetVScrollbar( pV );
			}
			pView->SetSkin( pBgCheckeredGreySkin );
			pView->SetCornerSkin( pPlateSkin );
			pDB->AddWidget( "view", pView );
		}

		return pDB;
	}

	//____ LoadBitmapFont() ____________________________________________________

	WgFontPtr LoadBitmapFont( const char * pImgPath, const char * pSpecPath, const WgSurfaceFactoryPtr& factory )
	{
		//TODO: This leaks memory until we have ref-counted

		WgSurfacePtr pFontImg = sdl_wglib::LoadSurface( pImgPath, factory );

		char * pFontSpec = (char*) LoadFile( pSpecPath );

		WgBitmapGlyphsPtr pGlyphs = WgBitmapGlyphs::Create( pFontImg, pFontSpec );

		WgFontPtr pFont = WgFont::Create();
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
