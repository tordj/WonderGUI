
#pragma once

#include "baseplate.h"

#include <wondergui.h>
#include <wg_freetypefont.h>

#include <initializer_list>
#include <string>
#include <sstream>

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
	Widget_p	createLowerPanel();
	Widget_p	createLogPanel();
	Widget_p	createNavigationPanel();

	void		selectAndLoadStream();

	bool		loadStream(string path);

	void		setupScreens();
	void		updateGUIAfterReload();

	void		setFrame( int frame );
	
	void		showFrameLog();
	void		showFullLog();
	void		showResources();
	void		showErrors();
	
private:

	void		_resetStream();							// Clear displays, delete surfaces.
	void		_playFrames( int begin, int end );
	void		_logFrames( int begin, int end );
	void		_updateFrameCounterAndSlider();
	void		_logFullStream();

	bool		_setupGUI(Visitor* pVisitor);
	bool		_loadSkins(Visitor* pVisitor);
	ScrollPanel_p	_standardScrollPanel();

	Visitor * 			m_pAppVisitor = nullptr;
	Window_p			m_pWindow;

	Blob_p				m_pStreamBlob;
	GfxStreamWrapper_p	m_pStreamWrapper;
	GfxStreamPlayer_p	m_pStreamPlayer;
	GfxStreamLogger_p	m_pStreamLogger;
	GfxStreamPump_p		m_pStreamPump;

	
	vector<GfxStream::iterator>	m_frames;
	vector<Surface_p>	m_screens;

	int					m_currentFrame = 0;			// First frame is 0.

	PackPanel_p			m_pDisplayToggles;
	PackPanel_p			m_pScreenLineup;
	Capsule_p			m_pLogCapsule;
	
	Widget_p			m_pFrameLogContainer;
	TextEditor_p		m_pFrameLogDisplay;

	Widget_p			m_pFullLogContainer;
	TextEditor_p		m_pFullLogDisplay;
	
	Widget_p			m_pResourceContainer;
	PackPanel_p			m_pResourcePanel;

	Widget_p			m_pErrorsContainer;
	TextEditor_p		m_pErrorsDisplay;

	TextDisplay_p		m_pProgressText;
	Slider_p			m_pProgressSlider;


	

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
