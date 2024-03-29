
#pragma once

#include <wonderapp.h>

#include <wondergui.h>
#include <wg_freetypefont.h>

#include <initializer_list>
#include <string>

using namespace wg;

class MyApp : public WonderApp
{
public:

	bool		init(Visitor* pVisitor) override;
	bool		update() override;
	void		exit() override;

	Widget_p 	createTopBar();
	Widget_p	createImagePanel();
	Widget_p	createInfoPanel();

	void		selectAndLoadImage();

	bool		loadImage(int idx);


private:

	bool			_setupGUI(Visitor* pVisitor);
	bool			_loadSkins(Visitor* pVisitor);

	WonderApp::Visitor * m_pAppVisitor = nullptr;
	Window_p			m_pWindow = nullptr;

	SurfaceDisplay_p	m_pImageDisplay;
	TextDisplay_p		m_pPathDisplay;
	Button_p			m_pPrevButton;
	Button_p			m_pNextButton;

	std::vector<std::string>	m_imagePaths;
	
	int				m_imageIdx;

	Skin_p			m_pPlateSkin;
	Skin_p			m_pButtonSkin;
	Skin_p			m_pToggleButtonSkin;
	Skin_p			m_pCheckBoxSkin;
	Skin_p			m_pSectionSkin;

	PackLayout_p	m_pLayout;
	
	TextStyle_p		m_pTextStyle;
	TextStyle_p		m_pLabelStyle;

	TextLayout_p	m_pTextLayoutCentered;
};
