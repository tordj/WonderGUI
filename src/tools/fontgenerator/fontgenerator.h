
#pragma once

#include <wonderapp.h>

#include <wondergui.h>
#include <wg_freetypefont.h>

#include <initializer_list>
#include <string>


class MyApp : public WonderApp
{
public:
	wg::Size	startWindowSize();

	bool		init(Visitor* pVisitor);
	bool		update();
	void		exit();

	wg::Widget_p 	createInputPanel();
	wg::Widget_p	createCharsPanel();
	wg::Widget_p	createOutputPanel();

	bool		loadTTF( const std::string& path );
	bool 		selectAndLoadTTF();

	bool		saveBitmapFont();

	bool		generateBitmapFont();
	bool		generateFontSpec( wg::FreeTypeFont * pFont, wg::String& charmap );
	bool		generateFontSurface( wg::FreeTypeFont * pFont, wg::String& charmap );
	void		displayBitmapFont();

private:

	bool			_setupGUI(Visitor* pVisitor);
	bool			_loadSkins(Visitor* pVisitor);

	Visitor *			m_pAppVisitor = nullptr;
	Window_p			m_pWindow;

	wg::TextDisplay_p	m_pTTFPathDisplay;
	wg::SelectBox_p		m_pSizeSelector;
	wg::SelectBox_p		m_pModeSelector;
	wg::SelectBox_p		m_pStemDarkeningSelector;
	wg::SelectBox_p		m_pImageFormatSelector;

	
	wg::TextEditor_p	m_pCharsEditor;
	wg::Blob_p			m_pLoadedFontBlob;
	
	wg::Surface_p		m_pBitmapFontSurface;
	std::string			m_bitmapFontSpec;
	wg::Surface_p		m_pFontDisplaySurface;
	wg::Image_p			m_pBitmapDisplay;
	
	wg::Skin_p			m_pPlateSkin;
	wg::Skin_p			m_pButtonSkin;
	wg::Skin_p			m_pToggleButtonSkin;
	wg::Skin_p			m_pCheckBoxSkin;
	wg::Skin_p			m_pSectionSkin;

	wg::PackLayout_p	m_pLayout;
	
	wg::TextStyle_p		m_pTextStyle;
	wg::TextStyle_p		m_pLabelStyle;

	wg::TextLayout_p	m_pTextLayoutCentered;
	wg::PackPanel_p		m_pList;
};
