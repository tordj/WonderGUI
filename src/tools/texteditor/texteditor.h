
#pragma once

#include <wonderapp.h>

#include <wondergui.h>
#include <wg_freetypefont.h>

#include <initializer_list>
#include <string>

#include "EditorWindow.h"

using namespace wg;

class MyApp : public WonderApp
{
public:
	
	bool		init(Visitor* pVisitor) override;
	bool		update() override;
	void		exit() override;


	bool		setupGUI();
	bool		openFile(const std::string& path);
	bool 		createEditorWindow( const std::string& windowTitle );

	
	// These are made public for our subclasses.

	Button_p		createButton(const char* label );
	ScrollPanel_p	createScrollPanel();

	WonderApp::Visitor* m_pAppVisitor = nullptr;

	Skin_p			m_pPlateSkin;
	Skin_p			m_pButtonSkin;
	Skin_p			m_pToggleButtonSkin;
	Skin_p			m_pCheckBoxSkin;
	Skin_p			m_pSectionSkin;

	PackLayout_p	m_pLayout;

	TextStyle_p		m_pTextStyle;
	TextStyle_p		m_pLabelStyle;

	TextLayout_p	m_pTextLayoutCentered;

	
private:

	bool			_loadSkins(Visitor* pVisitor);
	
	std::vector<EditorWindow_p>	m_editorWindows;
	
};
