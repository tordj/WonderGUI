
#pragma once

#include <wonderapp.h>
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
	void		skipFrames(int frames);

	void		showFrameLog();
	void		showFullLog();

	void		showOptimizerInLog();
	void		showOptimizerOutLog();

	void		showResources();
	void		showStatistics();
	void		showErrors();

	void		toggleDebugRects(bool bShow);
	
private:

	void		_resetStream();							// Clear displays, delete surfaces.
	void		_playFrames( int begin, int end, bool bOptimize );
	void		_logFrames( int begin, int end, bool bOptimize, TextEditor * pDisplay );
	void		_updateFrameCounterAndSlider();
	void		_logFullStream();
	void		_updateResourcesView();
	void		_updateDebugOverlays();

	bool		_setupGUI(Visitor* pVisitor);
	bool		_loadSkins(Visitor* pVisitor);
	
	void		_generateFrameStatistics();
	void		_displayFrameStatistics();

	ScrollPanel_p	_standardScrollPanel();
	Widget_p		_buildSurfaceDisplayWithIndexTag( Surface * pSurf, int index );
	
	Visitor * 			m_pAppVisitor = nullptr;
	Window_p			m_pWindow;

	Blob_p				m_pStreamBlob;
	GfxStreamPlayer_p	m_pStreamPlayer;
	GfxStreamLogger_p	m_pStreamLogger;
	GfxStreamPump_p		m_pStreamPump;

	
	vector<GfxStream::iterator>	m_frames;
	vector<Surface_p>			m_screens;
	vector<Surface_p>			m_debugOverlays;
	vector<SurfaceDisplay_p>	m_overlayDisplays;
	vector<StackPanel_p>		m_screenStacks;

	
	class FrameStats
	{
	public:
		FrameStats()
		{
			for( int i = 0 ; i < CanvasRef_size ; i++ )
			{
				canvasRects[i] = 0;
				canvasPixels[i] = 0;
			}
		}
		
		int canvasRects[CanvasRef_size];
		int canvasPixels[CanvasRef_size];
	};
	
	vector<FrameStats>	m_frameStatistics;
	
	int					m_currentFrame = 0;			// First frame is 0.
	bool				m_bShowDebugRects = false;

	PackPanel_p			m_pDisplayToggles;
	PackPanel_p			m_pScreenLineup;
	Capsule_p			m_pLogCapsule;
	
	Widget_p			m_pFrameLogContainer;
	TextEditor_p		m_pFrameLogDisplay;

	Widget_p			m_pFullLogContainer;
	TextEditor_p		m_pFullLogDisplay;

	Widget_p			m_pOptimizerInLogContainer;
	TextEditor_p		m_pOptimizerInLogDisplay;

	Widget_p			m_pOptimizerOutLogContainer;
	TextEditor_p		m_pOptimizerOutLogDisplay;

	
	Widget_p			m_pResourceContainer;
	PackPanel_p			m_pResourcePanel;

	Widget_p			m_pErrorsContainer;
	TextEditor_p		m_pErrorsDisplay;

	Widget_p			m_pStatisticsContainer;
	TextEditor_p		m_pStatisticsDisplay;

	
	TextDisplay_p		m_pProgressText;
	Slider_p			m_pProgressSlider;

	ToggleButton_p		m_pDebugRectsToggle;

	
	int				m_imageIdx;

	Skin_p			m_pPlateSkin;
	Skin_p			m_pButtonSkin;
	Skin_p			m_pToggleButtonSkin;
	Skin_p			m_pCheckBoxSkin;
	Skin_p			m_pSectionSkin;
	Skin_p			m_pRedOutlinedBlackSkin;

	PackLayout_p	m_pLayout;
	
	Font_p			m_pFont;
	TextStyle_p		m_pTextStyle;
	TextStyle_p		m_pLabelStyle;
	TextStyle_p		m_pBigWhiteStyle;

	TextLayout_p	m_pTextLayoutCentered;

};
