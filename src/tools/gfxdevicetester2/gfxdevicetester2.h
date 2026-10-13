#pragma once

#include <wondergui.h>
#include <wonderapp.h>

#include <chrono>
#include <string>
#include <vector>

#include "testsuites/testsuite.h"

using namespace wg;
using namespace std;


//____ GfxDeviceTester2 _______________________________________________________
/*
	Runs the test suites on two backends and compares them:

		Testee		- The native GfxDevice provided by the WonderApp framework.
		Reference	- GfxDeviceGen2 with SoftBackend, our reference implementation.

	The window has three views:

		Display		- Shows testee, reference and their difference for one test,
					  picked from a tree of all test suites.
		Accuracy	- Compares every test on both backends and lists the results.
		Performance	- Times every test on the testee.
*/

class GfxDeviceTester2 : public WonderApp
{
	friend class WonderApp;
public:

	bool	init(wapp::API* pAPI) override;
	bool	update() override;
	void	exit() override;

	void	closeWindow(wapp::Window* pWindow) override;

protected:
	GfxDeviceTester2() {}
	~GfxDeviceTester2() {}

	enum class View
	{
		Display,
		Accuracy,
		Performance
	};

	enum class Background
	{
		Black,
		White,
		Grey,
		Checkered
	};

	enum Side
	{
		TESTEE = 0,
		REFERENCE = 1
	};

	// A GfxDevice and the canvas the tests render to.

	struct Backend
	{
		string			name;
		GfxDevice_p		pDevice;
		Surface_p		pCanvas;
	};

	struct SuiteEntry
	{
		string			name;
		bool			bWorking = false;					// Initialized on both backends.
		TestSuite*		pSuites[2] = { nullptr, nullptr };	// One instance per backend, they hold backend specific resources.
		int				firstTest = 0;
		int				nbTests = 0;
		DrawerPanel_p	pTreeDrawer;
	};

	struct AccuracyResult
	{
		bool	bDone = false;
		bool	bFailed = false;							// Couldn't be rendered or read back.
		int		maxDiff = 0;								// Largest difference in any channel, 0-255.
		double	meanDiff = 0;								// Mean difference over all channels of all pixels.
		int		nbOverThreshold = 0;						// Channel values differing by more than c_diffThreshold.
		CoordI	worst;										// First pixel with the largest difference.
	};

	struct PerformanceResult
	{
		bool	bDone = false;
		bool	bFailed = false;
		double	secondsPerRun = 0;
		int		loops = 0;									// Calls to run() per measurement.
	};

	struct TestEntry
	{
		string				name;
		int					suite = 0;
		Test*				pTests[2] = { nullptr, nullptr };	// Testee, followed by Reference.

		AccuracyResult		accuracy;
		PerformanceResult	performance;

		Widget_p			pTreeEntry;
		TextDisplay_p		pPerfTimeCell;
		TextDisplay_p		pPerfLoopsCell;
	};

	// Testee, reference and diff boxes for one test, with a slider for amplifying the diff.

	struct Viewer
	{
		TextDisplay_p		pTitle;							// Suite and name of the test shown.
		Surface_p			pImages[3];						// Testee, reference and diff, as displayed.
		Slider_p			pDiffSlider;
		TextDisplay_p		pDiffFactorLabel;

		int					test = -1;
		Background			background = Background::Black;
		int					diffFactor = 1;
		vector<uint8_t>		pixels[2];						// Testee and reference read back as ARGB_8.
	};

	// A test differs if any channel value differs by more than c_diffThreshold. If no more
	// than c_maxSpecks values do, the difference is reported as specks: isolated pixels
	// where backends may legitimately round differently, like samples right on a pixel edge.

	static const int	c_diffThreshold = 16;
	static const int	c_maxSpecks = 64;

	static const int	c_maxDiffFactor = 10;

	// Performance measurements. Calls to run() are looped until a measurement takes at
	// least c_perfTargetTime, then the fastest of c_perfRounds measurements is kept
	// (c_perfSlowRounds for tests taking more than 10 times c_perfTargetTime).

	static constexpr double	c_perfTargetTime = 0.02;
	static const int		c_perfRounds = 5;
	static const int		c_perfSlowRounds = 3;
	static const int		c_perfMaxLoops = 100000;

	static constexpr double	c_progressBudget = 0.05;		// Seconds of accuracy testing per update.

	using Clock = std::chrono::steady_clock;

	//

	void		setup_backends();
	void		setup_tests();
	void		add_testsuite(TestSuite* pTesteeSuite, TestSuite* pReferenceSuite);
	void		destroy_tests();

	bool		setup_chrome();
	Widget_p	create_display_view();
	Widget_p	create_accuracy_view();
	Widget_p	create_performance_view();
	Widget_p	create_test_tree();
	Widget_p	create_progress_bar(Slider_p& pBar, TextDisplay_p& pLabel);
	void		create_viewer(Viewer& viewer);
	Widget_p	create_image_box(Viewer& viewer, int image, float zoom);

	void		select_view(View view);

	// Rendering and comparing

	bool		render_test(Side side, int test, Background background);
	void		fill_background(GfxDevice* pDevice, Background background);
	bool		read_canvas(Side side, vector<uint8_t>& pixels);
	void		write_image(Surface* pSurface, const vector<uint8_t>& pixels);
	void		compare(const vector<uint8_t>& testee, const vector<uint8_t>& reference, AccuracyResult& result);

	void		show_test(Viewer& viewer, int test);
	void		refresh_viewer(Viewer& viewer);
	void		refresh_diff(Viewer& viewer);

	void		show_in_display(int test);

	// Accuracy

	void		start_accuracy_run();
	void		continue_accuracy_run();
	void		run_accuracy_test(int test);
	void		refresh_accuracy_table();
	void		select_accuracy_row(int test);

	// Performance

	void		start_performance_run();
	void		continue_performance_run();
	void		run_performance_test(int test);
	double		time_test(Test* pTest, int loops);
	void		sync_testee();
	void		refresh_performance_row(int test);

	static string	format_time(double seconds);

	//

	const SizeI			m_canvasSize = { 512, 512 };

	wapp::API*			m_pAPI = nullptr;
	wapp::Window_p		m_pWindow;

	Backend				m_backends[2];						// Testee, followed by Reference.
	ColorSpace			m_compareColorSpace = ColorSpace::sRGB;

	vector<SuiteEntry>	m_suites;
	vector<TestEntry>	m_tests;

	vector<RouteId>		m_routes;

	// Chrome

	PackLayout_p		m_pLayout;
	TextLayout_p		m_pRightAligned;

	ToggleButton_p		m_pViewButtons[3];
	PaddingCapsule_p	m_pViewBase;
	Widget_p			m_pViews[3];

	TextStyle_p			m_pOkStyle;
	TextStyle_p			m_pSpecksStyle;
	TextStyle_p			m_pDiffStyle;

	// Display view

	Viewer				m_displayViewer;
	SelectCapsule_p		m_pTestTreeSelector;

	// Accuracy view

	Viewer				m_accuracyViewer;
	TablePanel_p		m_pAccuracyTable;
	TextDisplay_p		m_pAccuracySummary;
	Button_p			m_pAccuracyRunButton;
	Slider_p			m_pAccuracyProgress;
	TextDisplay_p		m_pAccuracyProgressLabel;
	vector<int>			m_accuracyRowOrder;					// Test index of each table row, after the header.
	int					m_accuracySelected = -1;

	bool				m_bAccuracyRunning = false;
	int					m_accuracyNext = 0;

	// Performance view

	TablePanel_p		m_pPerformanceTable;
	TextDisplay_p		m_pPerformanceSummary;
	Button_p			m_pPerformanceRunButton;
	Slider_p			m_pPerformanceProgress;
	TextDisplay_p		m_pPerformanceProgressLabel;

	bool				m_bPerformanceRunning = false;
	int					m_performanceNext = 0;
};
