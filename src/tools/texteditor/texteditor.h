
#pragma once

#include <wonderapp.h>

#include <wondergui.h>
#include <wg_freetypefont.h>

#include <initializer_list>
#include <string>

#include "editorwindow.h"

using namespace wg;

class MyApp : public WonderApp
{
public:
	
	bool		init(Visitor* pVisitor) override;
	bool		update() override;
	void		exit() override;


	bool		setupGUI();
	bool		openFile(const std::string& path);
	bool 		createEditorWindow( const std::string& windowTitle, const std::string& path );
	
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
	TextStyle_p		m_pEditorStyle;

	TextLayout_p	m_pTextLayoutCentered;

	
private:

	bool			_loadSkins(Visitor* pVisitor);
	
	std::string		_createWindowTitle( const std::string& path );
	
	std::vector<EditorWindow_p>	m_editorWindows;
	
};
