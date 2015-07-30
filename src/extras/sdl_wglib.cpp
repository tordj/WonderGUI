#include <sdl_wglib.h>
#include <wg_sdlsurface.h>
#include <wondergui.h>


namespace sdl_wglib
{
	static int				g_ticks = 0;

	//____ MapKeys() ___________________________________________________________

	void MapKeys()
	{
		WgBase::mapKey( WG_KEY_SHIFT, SDLK_LSHIFT );
		WgBase::mapKey( WG_KEY_SHIFT, SDLK_RSHIFT );
		WgBase::mapKey( WG_KEY_CONTROL, SDLK_LCTRL );
		WgBase::mapKey( WG_KEY_CONTROL, SDLK_RCTRL );
		WgBase::mapKey( WG_KEY_ALT, SDLK_LALT );
		WgBase::mapKey( WG_KEY_ALT, SDLK_RALT );

		WgBase::mapKey( WG_KEY_LEFT, SDLK_LEFT );
		WgBase::mapKey( WG_KEY_RIGHT, SDLK_RIGHT );
		WgBase::mapKey( WG_KEY_UP, SDLK_UP );
		WgBase::mapKey( WG_KEY_DOWN, SDLK_DOWN );

		WgBase::mapKey( WG_KEY_HOME, SDLK_HOME );
		WgBase::mapKey( WG_KEY_END, SDLK_END );
		WgBase::mapKey( WG_KEY_PAGE_UP, SDLK_PAGEUP );
		WgBase::mapKey( WG_KEY_PAGE_DOWN, SDLK_PAGEDOWN );

		WgBase::mapKey( WG_KEY_RETURN, SDLK_RETURN );
		WgBase::mapKey( WG_KEY_BACKSPACE, SDLK_BACKSPACE );
		WgBase::mapKey( WG_KEY_DELETE, SDLK_DELETE );
		WgBase::mapKey( WG_KEY_TAB, SDLK_TAB );
		WgBase::mapKey( WG_KEY_ESCAPE, SDLK_ESCAPE );
	}

	//____ BeginEvents() _______________________________________________________

	void BeginEvents( const WgEventHandlerPtr& pHandler )
	{
		// Add a tick event first as the first.

		int ticks = SDL_GetTicks();
		pHandler->QueueEvent( WgTickEvent::create( ticks - g_ticks ) );
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
				pHandler->QueueEvent( WgKeyPressEvent::create( event.key.keysym.sym ) );
				if( event.key.keysym.unicode != 0 )
					pHandler->QueueEvent( WgCharacterEvent::create( event.key.keysym.unicode ) );
				break;
			}

			case SDL_KEYUP:
			{
				pHandler->QueueEvent( WgKeyReleaseEvent::create( event.key.keysym.sym ) );
				break;
			}

			case	SDL_MOUSEMOTION:
			{
				pHandler->QueueEvent( WgMouseMoveEvent::create( WgCoord( event.motion.x, event.motion.y ) ) );
				break;
			}

			case	SDL_MOUSEBUTTONDOWN:
				if(event.button.button == 4 )
					pHandler->QueueEvent( WgWheelRollEvent::create( 1, 1 ) );
				else if(event.button.button == 5)
					pHandler->QueueEvent( WgWheelRollEvent::create( 1, -1 ) );
				else
				{
					pHandler->QueueEvent( WgMousePressEvent::create( (WgMouseButton) event.button.button ) );
				}
				break;

			case	SDL_MOUSEBUTTONUP:
				if( event.button.button != 4 && event.button.button != 5 )
					pHandler->QueueEvent( WgMouseReleaseEvent::create( (WgMouseButton) event.button.button ) );
				break;
		}
	}

	//_____ EndEvents() ________________________________________________________

	void EndEvents( const WgEventHandlerPtr& pHandler )
	{
		pHandler->ProcessEvents();
	}

	//____ loadSurface() __________________________________________________________

	WgSurface_p loadSurface( const char * path, const WgSurfaceFactory_p& pFactory )
	{
		SDL_Surface* bmp = IMG_Load(path);
		if (!bmp)
		{
			printf("Unable to load bitmap: %s\n", IMG_GetError());
			return 0;
		}

		WgSDLSurfacePtr	wrapper = WgSDLSurface::create( bmp );

		WgSurface_p pSurf = pFactory->createSurface( wrapper->size(), wrapper->isOpaque()? WG_PIXEL_RGB_8 : WG_PIXEL_ARGB_8 );

		if( !pSurf )
		{
			printf("Unable to create surface for loaded bitmap: %s\n", path);
			return 0;
		}

		if( !pSurf->copyFrom( wrapper, WgCoord(0,0) ) )
		{
			printf("Unable to copy loaded bitmap '%s' to surface.\n", path);
			return 0;
		};

		return pSurf;
	}

	//____ LoadStdWidgets() _____________________________________________________

	WgResDB_p LoadStdWidgets( const char * pImagePath, const WgSurfaceFactory_p& pFactory )
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
		const int LISTENTRY_OFS			= COMBOBOX_OFS + 35;
		const int SORT_ARROW_OFS		= LISTENTRY_OFS + 9;
		const int SMILEY_OFS			= SORT_ARROW_OFS + 8;
		const int TILES_OFS				= 256+192;

		WgSurface_p pSurface = loadSurface( pImagePath, pFactory );
		if( !pSurface )
			return 0;

		WgBlockSkin_p	pHScrollbarBwdButtonSkin = WgBlockSkin::createClickable( pSurface, WgSize(17,17), WgCoord(1,HSLIDER_BTN_OFS), WgSize(19,0), WgBorder(3) );
		pHScrollbarBwdButtonSkin->setContentPadding(WgBorder(4));

		WgBlockSkin_p	pHScrollbarFwdButtonSkin = WgBlockSkin::createClickable( pSurface, WgSize(17,17), WgCoord(77,HSLIDER_BTN_OFS), WgSize(19,0), WgBorder(3) );
		pHScrollbarFwdButtonSkin->setContentPadding(WgBorder(4));

		WgBlockSkin_p	pVScrollbarBwdButtonSkin = WgBlockSkin::createClickable( pSurface, WgSize(17,17), WgCoord(1,VSLIDER_BTN_OFS), WgSize(19,0), WgBorder(3) );
		pVScrollbarBwdButtonSkin->setContentPadding(WgBorder(4));

		WgBlockSkin_p	pVScrollbarFwdButtonSkin = WgBlockSkin::createClickable( pSurface, WgSize(17,17), WgCoord(77,VSLIDER_BTN_OFS), WgSize(19,0), WgBorder(3) );
		pVScrollbarFwdButtonSkin->setContentPadding(WgBorder(4));

		WgBlockSkin_p	pScrollbarHandleSkin = WgBlockSkin::createClickable( pSurface, WgSize(8,8), WgCoord(1,SLIDER_OFS), WgSize(10,0), WgBorder(2) );
		pScrollbarHandleSkin->setContentPadding(WgBorder(3));

		WgBlockSkin_p	pScrollbarBackSkin = WgBlockSkin::createStatic( pSurface, WgRect(1,SLIDER_BACK_OFS,5,5), WgBorder(2) );
		pScrollbarBackSkin->setContentPadding(WgBorder(2));

//		WgBlocksetPtr pResizeButtonBlocks = WgBlockset::CreateFromRow(pSurface, WgRect(1,RESIZE_BUTTON_OFS,86,20), 4, 2, WG_OPAQUE);
//		pResizeButtonBlocks->setFrame(WgBorder(3));

		WgBlockSkin_p	pCheckboxIconSkin = WgBlockSkin::createClickSelectableWidget( pSurface, WgSize(11,11), WgCoord(1,CHECKBOX_OFS), WgSize(13,0), WgBorder(2) );
		pCheckboxIconSkin->setContentPadding(WgBorder(3));

		WgBlockSkin_p	pRadiobuttonIconSkin = WgBlockSkin::createClickSelectableWidget( pSurface, WgSize(11,11), WgCoord(1,RADIOBUTTON_OFS), WgSize(13,0) );
		pRadiobuttonIconSkin->setContentPadding(WgBorder(3));

		WgBlockSkin_p pButtonSkin = WgBlockSkin::createClickable( pSurface, WgSize(8,8), WgCoord(1,BUTTON_OFS), WgSize(10,0), WgBorder(3) );
		pButtonSkin->setContentPadding(WgBorder(4));
		pButtonSkin->setContentShift( WG_STATE_PRESSED, WgCoord(2,2) );

		WgBlockSkin_p pHSplitSkin = WgBlockSkin::createStatic(pSurface, WgRect(1,SPLITS_AND_FRAME_OFS,8,2), WgBorder(0) );

		WgBlockSkin_p pVSplitBlocks = WgBlockSkin::createStatic(pSurface, WgRect(11,SPLITS_AND_FRAME_OFS,2,8), WgBorder(0) );

		WgBlockSkin_p pFrameSkin = WgBlockSkin::createClickable( pSurface, WgSize(8,8), WgCoord(1, SPLITS_AND_FRAME_OFS), WgSize(10,0), WgBorder(2) );
		pFrameSkin->setContentPadding(WgBorder(3));
		
//		WgBlocksetPtr pComboboxBlocks = WgBlockset::CreateFromRow(pSurface, WgRect(1,COMBOBOX_OFS,98,20), 4, 2, WG_OPAQUE);
//		pComboboxBlocks->setFrame(WgBorder(1,1,20,1));
//		pComboboxBlocks->setPadding(WgBorder(2,2,21,2));

		WgBlockSkin_p pListEntrySkin = WgBlockSkin::createClickSelectable( pSurface, WgSize(8,6), WgCoord(1,LISTENTRY_OFS), WgSize(10,0), WgBorder(1) );
		pListEntrySkin->setContentPadding(WgBorder(4));

		WgBlockSkin_p pSortArrowSkin = WgBlockSkin::createClickSelectable( pSurface, WgSize(11,6), WgCoord(1,SORT_ARROW_OFS), WgSize(13,0), WgBorder(0) );

		WgBlockSkin_p pSmileyIconSkin = WgBlockSkin::createClickSelectable( pSurface, WgSize(12,12), WgCoord(1, SMILEY_OFS), WgSize(14,0), WgBorder(0) );


		WgBlockSkin_p pBgCheckeredGreySkin = WgBlockSkin::createStatic( pSurface, WgRect(0,TILES_OFS,64,64) );
		pBgCheckeredGreySkin->setTiledCenter(true);
		pBgCheckeredGreySkin->optimizeRenderMethods();

		WgBlockSkin_p pBgBlueGradientSkin = WgBlockSkin::createStatic( pSurface, WgRect(1*64,TILES_OFS,64,64) );
		pBgBlueGradientSkin->optimizeRenderMethods();


//		WgColorsetPtr pSelectionColors = WgColorset::create( WgColor(0x0), WgColor(0x40FFFFFF), WgColor(0x80FFFFFF), WgColor(0x40000000), WgColor(0x0) );

		WgBlockSkin_p pPlateSkin = WgBlockSkin::createClickable( pSurface, WgSize(8,8), WgCoord(1,PLATE_OFS), WgSize(10,0), WgBorder(3) );
		pPlateSkin->setContentPadding(WgBorder(4));
		pPlateSkin->optimizeRenderMethods();

		WgResDB_p pDB = WgResDB::create();

		pDB->addSkin( "listentry", pListEntrySkin );
		pDB->addSkin( "plate", pPlateSkin );
		pDB->addSkin( "sortarrow", pSortArrowSkin );
		pDB->addSkin( "smiley", pSmileyIconSkin );

		// Create standard button

		WgButton_p pButton = WgButton::create();
		pButton->setSkin( pButtonSkin );
		pDB->addWidget( "button", pButton );

		// Create standard plate

		WgImage_p pPlate = WgImage::create();
		pPlate->setSkin( pPlateSkin );
		pDB->addWidget( "plate", pPlate );

		// Create standard checkbox

		WgCheckBoxPtr pCheckbox = WgCheckBox::create();
		pCheckbox->Icon()->set( pCheckboxIconSkin );
		pDB->addWidget( "checkbox", pCheckbox );

		// Create standard radiobutton

		WgRadioButtonPtr pRadiobutton = WgRadioButton::create();
		pRadiobutton->Icon()->set( pRadiobuttonIconSkin );
		pDB->addWidget( "radiobutton", pRadiobutton );

		// Create standard horizontal scrollbar

		WgScrollbar_p pHScrollbar = WgScrollbar::create();
		pHScrollbar->SetSkins( pScrollbarBackSkin, pScrollbarHandleSkin, pHScrollbarBwdButtonSkin, pHScrollbarFwdButtonSkin );
		pDB->addWidget( "hscrollbar", pHScrollbar );

		// Create standard vertical scrollbar

		WgScrollbar_p pVScrollbar = WgScrollbar::create();
		pVScrollbar->SetSkins( pScrollbarBackSkin, pScrollbarHandleSkin, pVScrollbarBwdButtonSkin, pVScrollbarFwdButtonSkin );
		pDB->addWidget( "vscrollbar", pVScrollbar );

		// Create standard menubar
/*
		WgMenubar * pMenubar = new WgMenubar();
		pMenubar->SetBgSource( pPlateBlocks );
		pDB->addWidget( "menubar", pMenubar );
*/
		// Create Background bitmaps

		WgImage_p pBgCheckeredGrey = WgImage::create();
		pBgCheckeredGrey->setSkin( pBgCheckeredGreySkin );
		pDB->addWidget( "bg_checkered_grey", pBgCheckeredGrey );

		WgImage_p pBgBlueGradient = WgImage::create();
		pBgBlueGradient->setSkin( pBgBlueGradientSkin );
		pDB->addWidget( "bg_blue_gradient", pBgBlueGradient );

		// Create standard menu

		WgMenu_p pMenu = WgMenu::create();
		pMenu->setSkin( pPlateSkin, 16, 16 );
		pMenu->SetSeparatorSkin( pHSplitSkin, WgBorder(1) );
		pMenu->SetCheckBoxSkin( pCheckboxIconSkin );
		pMenu->SetRadioButtonSkin( pRadiobuttonIconSkin );
		pMenu->SetScrollbarSkins( pScrollbarBackSkin, pScrollbarHandleSkin, pVScrollbarBwdButtonSkin, pVScrollbarFwdButtonSkin );
		pMenu->setEntrySkin( pPlateSkin );
		pDB->addWidget( "menu", pMenu );

		// Create standard combobox
/*
		WgCombobox * pCombobox = new WgCombobox();
		pCombobox->SetSource( pComboboxBlocks );
		pDB->addWidget( "combobox", pCombobox );
*/
		// Create standard view
		
		{
			WgScrollPanel_p pView = WgScrollPanel::create();
			
			WgWidget_p pHScrollbar = pDB->cloneWidget( "hscrollbar" );
			WgWidget_p pVScrollbar = pDB->cloneWidget( "vscrollbar" );

			if( pHScrollbar )
			{
				WgScrollbar_p pH = WgScrollbar::cast(pHScrollbar);
				pView->setHorizontalScrollbar( pH );
			}
			if( pVScrollbar )
			{
				WgScrollbar_p pV = WgScrollbar::cast(pVScrollbar);
				pView->setVerticalScrollbar( pV );
			}
			pView->setSkin( pBgCheckeredGreySkin );
			pView->setCornerSkin( pPlateSkin );
			pDB->addWidget( "view", pView );
		}

		return pDB;
	}

	//____ LoadBitmapFont() ____________________________________________________

	WgFont_p LoadBitmapFont( const char * pImgPath, const char * pSpecPath, const WgSurfaceFactory_p& factory )
	{
		//TODO: This leaks memory until we have ref-counted

		WgSurface_p pFontImg = sdl_wglib::loadSurface( pImgPath, factory );

		char * pFontSpec = (char*) LoadFile( pSpecPath );

		WgBitmapGlyphs_p pGlyphs = WgBitmapGlyphs::create( pFontImg, pFontSpec );

		WgFont_p pFont = WgFont::create();
		pFont->setBitmapGlyphs( pGlyphs, WG_STYLE_NORMAL, 0 );

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
