
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

	struct EditorWindow
	{
		WonderApp::Window_p	pWindow;
		RootPanel_p			pRootPanel;
		std::string			path;
		TextEditor_p		pEditor;
		
	};
	
	bool		init(Visitor* pVisitor) override;
	bool		update() override;
	void		exit() override;


	bool		setupGUI();
	bool		openFile(const std::string& path);
	bool 		createEditorWindow( const std::string& windowTitle );

	bool		setupWindowGUI(EditorWindow& instance);
	

	Widget_p 	createTopBar();
	Widget_p	createInfoPanel();

	void		selectAndLoadImage();

	
private:

	bool			_loadSkins(Visitor* pVisitor);

	WonderApp::Visitor * m_pAppVisitor = nullptr;
	WonderApp::Window_p m_pWindow = nullptr;


	
	std::vector<EditorWindow>	m_editorWindows;
	
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
