#include "sdl_wglib.h"
//#include <wg_surface_sdl.h>
#include <wg3_softsurface.h>
#include <wg3_softsurfacefactory.h>
#include <wondergui.h>
#include <wg_resdb.h>

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

	bool g_bStopScroll = false;

	void BeginEvents( WgEventHandler * pHandler )
	{
		g_pHandler = pHandler;

		// Add a tick event first as the first.

		int ticks = SDL_GetTicks();
		if( !g_bStopScroll )
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
				if( event.key.keysym.sym == SDLK_RETURN )
                    g_pHandler->QueueEvent( new WgEvent::Character( 13 ) );
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

				if (event.button.button == 2)
					g_bStopScroll = !g_bStopScroll;
				if (event.button.button == 3)
				{
					g_pHandler->QueueEvent(new WgEvent::Tick(15));
				}

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
			case SDL_TEXTINPUT:
				g_pHandler->QueueEvent(new WgEvent::Character((short)event.text.text[0]));
				break;

		}
	}

	//_____ EndEvents() ________________________________________________________

	void EndEvents()
	{
		g_pHandler->ProcessEvents();
	}

	//____ SavePNG() ________________________________________________________

    bool SavePNG(wg::Surface * pSurface, const char * path)
	{
		WgSize size = pSurface->size();
		const WgPixelFormat * pFmt = pSurface->pixelDescription();

		SDL_Surface * pOutput = SDL_CreateRGBSurface(0, size.w, size.h, pFmt->bits, pFmt->R_mask, pFmt->G_mask, pFmt->B_mask, pFmt->A_mask);


		int err = SDL_LockSurface(pOutput);
		if (err != 0)
			return false;

        char * pSrcPixels = (char *)pSurface->lock(WgAccessMode::ReadOnly);

		for (int y = 0; y < size.h; y++)
		{
			char * pDest = ((char *) pOutput->pixels) + pOutput->pitch * y;
			char * pSource = pSrcPixels + pSurface->pitch() * y;
			memcpy(pDest, pSource, size.w * pFmt->bits / 8);
		}

		pSurface->unlock();

		SDL_UnlockSurface(pOutput);


		int err2 = IMG_SavePNG(pOutput, path);
		SDL_FreeSurface(pOutput);

		return true;
	}


	//____ LoadSurface() __________________________________________________________

    wg::Surface_p LoadSurface( const char * path, const wg::SurfaceFactory * factory )
	{
		SDL_Surface* bmp = IMG_Load(path);
		if (!bmp)
		{
			printf("Unable to load bitmap: %s\n", IMG_GetError());
			return 0;
		}

//		WgSurfaceSDL	wrapper( bmp );

		WgPixelType type;
		
		if( bmp->format->Amask == 0 )
            type = WgPixelType::BGR_8;
		else
            type = WgPixelType::BGRA_8;

		WgSize dimensions( bmp->w, bmp->h );

		WgPixelFormat	pixelFormat;
		ConvertPixelFormat( &pixelFormat, bmp->format );

        wg::Surface_p pSurf = factory->createSurface( dimensions, type, (uint8_t*) bmp->pixels, bmp->pitch, &pixelFormat );

		if( !pSurf )
		{
			printf("Unable to create surface for loaded bitmap: %s\n", path);
			return 0;
		}

		SDL_FreeSurface( bmp );
		return pSurf;
	}

	//____ LoadStdWidgets() _____________________________________________________

    WgResDB * LoadStdWidgets( const char * pImagePath, const char * pImagePathX2, const char * pImagePathX4, const wg::SurfaceFactory* factory )
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

        wg::Surface_p surfaces[3];

        wg::Surface_p pSurface = LoadSurface( pImagePath, factory );
		if( !pSurface )
			return 0;

		surfaces[0] = pSurface;
		surfaces[1] = pSurface;
		surfaces[2] = pSurface;

		if( pImagePathX2 )
		{
			auto pSurfaceX2 = LoadSurface( pImagePathX2, factory );
			if( !pSurfaceX2 )
				return 0;
				
			surfaces[1] = pSurfaceX2;
			surfaces[2] = pSurfaceX2;
		}


		if( pImagePathX4 )
		{
			auto pSurfaceX4 = LoadSurface( pImagePathX4, factory );
			if( !pSurfaceX4 )
				return 0;
				
			surfaces[2] = pSurfaceX4;
		}
		
		WgBlocksetPtr pHSliderBtnBwdBlocks	= WgBlockset::CreateFromRow(pSurface, WgRect(1,HSLIDER_BTN_OFS,74,17), 4, 2, WG_OPAQUE);
		pHSliderBtnBwdBlocks->SetFrame(WgBorders(3));
		pHSliderBtnBwdBlocks->SetPadding(WgBorders(4));

		WgBlocksetPtr pHSliderBtnFwdBlocks	= WgBlockset::CreateFromRow(pSurface, WgRect(77,HSLIDER_BTN_OFS,74,17), 4, 2, WG_OPAQUE);
		pHSliderBtnFwdBlocks->SetFrame(WgBorders(3));
		pHSliderBtnFwdBlocks->SetPadding(WgBorders(4));

		WgBlocksetPtr pVSliderBtnBwdBlocks	= WgBlockset::CreateFromRow(pSurface, WgRect(1,VSLIDER_BTN_OFS,74,17), 4, 2, WG_OPAQUE);
		pVSliderBtnBwdBlocks->SetFrame(WgBorders(3));
		pVSliderBtnBwdBlocks->SetPadding(WgBorders(4));

		WgBlocksetPtr pVSliderBtnFwdBlocks	= WgBlockset::CreateFromRow(pSurface, WgRect(77,VSLIDER_BTN_OFS,74,17), 4, 2, WG_OPAQUE);
		pVSliderBtnFwdBlocks->SetFrame(WgBorders(3));
		pVSliderBtnFwdBlocks->SetPadding(WgBorders(4));

		WgBlocksetPtr pSliderBlocks	= WgBlockset::CreateFromRow(pSurface, WgRect(1,SLIDER_OFS,38,8), 4, 2, WG_OPAQUE);
		pSliderBlocks->SetFrame(WgBorders(2));
		pSliderBlocks->SetPadding(WgBorders(3));

		WgBlocksetPtr pSliderBackBlocks	= WgBlockset::CreateFromRect(pSurface, WgRect(1,SLIDER_BACK_OFS,5,5), WG_OPAQUE );
		pSliderBackBlocks->SetFrame(WgBorders(2));
		pSliderBackBlocks->SetPadding(WgBorders(2));

		WgBlocksetPtr pResizeButtonBlocks = WgBlockset::CreateFromRow(pSurface, WgRect(1,RESIZE_BUTTON_OFS,86,20), 4, 2, WG_OPAQUE);
		pResizeButtonBlocks->SetFrame(WgBorders(3));

		WgBlocksetPtr pCheckboxUncheckedBlocks = WgBlockset::CreateFromRow(pSurface, WgRect(1,CHECKBOX_OFS,50,11), 4, 2, WG_OPAQUE);
		pCheckboxUncheckedBlocks->SetFrame(WgBorders(2));
		pCheckboxUncheckedBlocks->SetPadding(WgBorders(3));

		WgBlocksetPtr pCheckboxCheckedBlocks = WgBlockset::CreateFromRow(pSurface, WgRect(53,CHECKBOX_OFS,50,11), 4, 2, WG_OPAQUE);
		pCheckboxCheckedBlocks->SetFrame(WgBorders(2));
		pCheckboxCheckedBlocks->SetPadding(WgBorders(3));

		WgBlocksetPtr pRadiobuttonUncheckedBlocks = WgBlockset::CreateFromRow(pSurface, WgRect(1,RADIOBUTTON_OFS,50,11), 4, 2, WG_OPAQUE);
		pRadiobuttonUncheckedBlocks->SetPadding(WgBorders(3));

		WgBlocksetPtr pRadiobuttonCheckedBlocks = WgBlockset::CreateFromRow(pSurface, WgRect(53,RADIOBUTTON_OFS,50,11), 4, 2, WG_OPAQUE);
		pRadiobuttonCheckedBlocks->SetPadding(WgBorders(3));


		WgRect buttonRects[4] { WgRect(1,BUTTON_OFS,8,8), WgRect(1+10,BUTTON_OFS,8,8), WgRect(1+20,BUTTON_OFS,8,8), WgRect(1+30,BUTTON_OFS,8,8) };
		WgBorders buttonFrame(3);
		WgBorders buttonPadding(4);
		
		WgBlocksetPtr pButtonBlocks = WgBlockset::CreateFromRects(pSurface, buttonRects[0], buttonRects[1], buttonRects[2], buttonRects[3], WG_OPAQUE);
		pButtonBlocks->SetFrame(buttonFrame);
		pButtonBlocks->SetPadding(buttonPadding);
/*
		for( int i = 0 ; i < 2 ; i++ )
		{
			for( int r = 0 ; r < 4 ; r++ )
			{
				buttonRects[r].x *= 2;
				buttonRects[r].y *= 2;
				buttonRects[r].w *= 2;
				buttonRects[r].h *= 2;
			}
			
			buttonFrame = WgBorders( buttonFrame.left*2, buttonFrame.top*2, buttonFrame.right*2, buttonFrame.bottom*2 );
			buttonPadding = WgBorders( buttonPadding.left*2, buttonPadding.top*2, buttonPadding.right*2, buttonPadding.bottom*2 );
			
			pButtonBlocks->AddAlternative( WG_SCALE_BASE << (i+1), surfaces[i+1], buttonRects[0], buttonRects[1], buttonRects[2], buttonRects[3], buttonRects[3], 
					buttonFrame, buttonPadding, WgCoord(), WgCoord() );
		
		}
*/



		WgBlocksetPtr pPlateBlocks = WgBlockset::CreateFromRow(pSurface, WgRect(1,PLATE_OFS,38,8), 4, 2, WG_OPAQUE);
		pPlateBlocks->SetFrame(WgBorders(3));
		pPlateBlocks->SetPadding(WgBorders(4));

		WgBlocksetPtr pHSplitBlocks = WgBlockset::CreateFromRect(pSurface, WgRect(1,SPLITS_AND_FRAME_OFS,8,2), WG_OPAQUE);

		WgBlocksetPtr pVSplitBlocks = WgBlockset::CreateFromRect(pSurface, WgRect(11,SPLITS_AND_FRAME_OFS,2,8), WG_OPAQUE);

		WgBlocksetPtr pFrameBlocks = WgBlockset::CreateFromRect(pSurface, WgRect(1,SPLITS_AND_FRAME_OFS,8,8), WG_OPAQUE);
		pFrameBlocks->SetFrame(WgBorders(2));
		pFrameBlocks->SetPadding(WgBorders(3));

		WgBlocksetPtr pComboboxBlocks = WgBlockset::CreateFromRow(pSurface, WgRect(1,COMBOBOX_OFS,98,20), 4, 2, WG_OPAQUE);
		pComboboxBlocks->SetFrame(WgBorders(1,1,20,1));
		pComboboxBlocks->SetPadding(WgBorders(2,2,21,2));

		WgBlocksetPtr pBgCheckeredGreyBlocks = WgBlockset::CreateFromRect( pSurface, WgRect(0,TILES_OFS,64,64), WG_OPAQUE | WG_TILE_ALL );
		WgBlocksetPtr pBgBlueGradientBlocks = WgBlockset::CreateFromRect( pSurface, WgRect(1*64,TILES_OFS,64,64), WG_OPAQUE );

		WgColorsetPtr pSelectionColors = WgColorset::Create( WgColor(0x0), WgColor(0x40FFFFFF), WgColor(0x80FFFFFF), WgColor(0x40000000), WgColor(0x0) );


		WgResDB * pDB = new WgResDB();



		// Create standard button

		WgButton * pButton = new WgButton();
		pButton->SetSource( pButtonBlocks );
		pDB->AddWidget( "button", pButton );

		// Create standard plate

		WgImage * pPlate = new WgImage();
		pPlate->SetSource( pPlateBlocks );
		pDB->AddWidget( "plate", pPlate );

		// Create standard checkbox

		WgCheckBox * pCheckbox = new WgCheckBox();
		pCheckbox->SetIcons( pCheckboxUncheckedBlocks, pCheckboxCheckedBlocks );
		pDB->AddWidget( "checkbox", pCheckbox );

		// Create standard radiobutton

		WgRadioButton * pRadiobutton = new WgRadioButton();
		pRadiobutton->SetIcons( pRadiobuttonUncheckedBlocks, pRadiobuttonCheckedBlocks );
		pDB->AddWidget( "radiobutton", pRadiobutton );

		// Create standard horizontal slider

		WgHSlider * pHSlider = new WgHSlider();
		pHSlider->SetSource( pSliderBackBlocks, pSliderBlocks, pHSliderBtnBwdBlocks, pHSliderBtnFwdBlocks );
		pDB->AddWidget( "hslider", pHSlider );

		// Create standard vertical slider

		WgVSlider * pVSlider = new WgVSlider();
		pVSlider->SetSource( pSliderBackBlocks, pSliderBlocks, pVSliderBtnBwdBlocks, pVSliderBtnFwdBlocks );
		pDB->AddWidget( "vslider", pVSlider );

		// Create standard menubar
/*
		WgMenubar * pMenubar = new WgMenubar();
		pMenubar->SetBgSource( pPlateBlocks );
		pDB->AddWidget( "menubar", pMenubar );
*/
		// Create Background bitmaps

		WgImage * pBgCheckeredGrey = new WgImage();
		pBgCheckeredGrey->SetSource( pBgCheckeredGreyBlocks );
		pDB->AddWidget( "bg_checkered_grey", pBgCheckeredGrey );

		WgImage * pBgBlueGradient = new WgImage();
		pBgBlueGradient->SetSource( pBgBlueGradientBlocks );
		pDB->AddWidget( "bg_blue_gradient", pBgBlueGradient );

		// Create standard menu
/*
		WgMenu * pMenu = new WgMenu();
		pMenu->SetBgSource( pPlateBlocks, 16, 16 );
		pMenu->SetSeparatorSource( pHSplitBlocks, WgBorders(1) );
		pMenu->SetCheckBoxSource( pCheckboxUncheckedBlocks, pCheckboxCheckedBlocks);
		pMenu->SetRadioButtonSource( pRadiobuttonUncheckedBlocks, pRadiobuttonCheckedBlocks);
		pMenu->SetSliderSource( pSliderBackBlocks, pSliderBlocks, pVSliderBtnBwdBlocks, pVSliderBtnFwdBlocks );
		pMenu->SetTileColors( pSelectionColors );
		pDB->AddWidget( "menu", pMenu );
*/
		// Create standard combobox
/*
		WgCombobox * pCombobox = new WgCombobox();
		pCombobox->SetSource( pComboboxBlocks );
		pDB->AddWidget( "combobox", pCombobox );
*/
		// Create standard view
		
		{
			WgScrollPanel * pView = new WgScrollPanel();
			
			WgWidget * pHSlider = pDB->CloneWidget( "hslider" );
			WgWidget * pVSlider = pDB->CloneWidget( "vslider" );

			if( pHSlider )
				pView->SetHSlider( static_cast<WgHSlider*>(pHSlider) );
			if( pVSlider )
				pView->SetVSlider( static_cast<WgVSlider*>(pVSlider) );

			pView->SetFillerBlocks( pPlateBlocks );
			pDB->AddWidget( "view", pView );
		}

		return pDB;
	}

	//____ LoadBitmapFont() ____________________________________________________

    WgFont * LoadBitmapFont( const char * pImgPath, const char * pSpecPath, const wg::SurfaceFactory * factory )
	{
		//TODO: This leaks memory until we have ref-counted

		auto pFontImg = sdl_wglib::LoadSurface( pImgPath, factory );

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
	
	//____ ConvertPixelFormat() ______________________________________________________

	void ConvertPixelFormat( WgPixelFormat * pWGFormat, const SDL_PixelFormat * pSDLFormat )
	{
        pWGFormat->format = WgPixelType::Custom;
		pWGFormat->bits = pSDLFormat->BitsPerPixel;

		pWGFormat->R_mask = pSDLFormat->Rmask;
		pWGFormat->G_mask = pSDLFormat->Gmask;
		pWGFormat->B_mask = pSDLFormat->Bmask;
		pWGFormat->A_mask = pSDLFormat->Amask;

		pWGFormat->R_shift = pSDLFormat->Rshift;
		pWGFormat->G_shift = pSDLFormat->Gshift;
		pWGFormat->B_shift = pSDLFormat->Bshift;
		pWGFormat->A_shift = pSDLFormat->Ashift;

		pWGFormat->R_bits = 8 - pSDLFormat->Rloss;
		pWGFormat->G_bits = 8 - pSDLFormat->Gloss;
		pWGFormat->B_bits = 8 - pSDLFormat->Bloss;
		pWGFormat->A_bits = 8 - pSDLFormat->Aloss;

	}
	
	
};
