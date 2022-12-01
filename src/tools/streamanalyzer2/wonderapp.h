
#pragma once

#include "baseplate.h"

#include <wondergui.h>
#include <wg_freetypefont.h>

#include <initializer_list>
#include <string>

using namespace wg;
using namespace std;

class MyApp : public WonderApp
{
public:
	bool		init(Visitor* pVisitor) override;
	bool		update() override;
	void		exit() override;

	Widget_p 	createTopBar();
	Widget_p	createDisplayPanel();
	Widget_p	createControlsPanel();

	void		selectAndLoadStream();

	bool		loadStream(string path);

	void		setupScreens();
	void		updateGUIAfterReload();

private:

	bool			_setupGUI(Visitor* pVisitor);
	bool			_loadSkins(Visitor* pVisitor);


	Visitor * 			m_pAppVisitor = nullptr;
	Window_p			m_pWindow;

	Blob_p				m_pStreamBlob;
	vector<GfxStream::iterator>	m_frames;
	vector<Surface_p>	m_screens;




	PackPanel_p			m_pDisplayToggles;
	PackPanel_p			m_pScreenLineup;


	Button_p			m_pPrevButton;
	Button_p			m_pNextButton;
	
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
