

#include <wondergui.h>

#include "app.h"

#include "device.h"
#include "testsuites/testsuite.h"



class GfxDeviceTester : public SimpleApp
{
public:

	GfxDeviceTester();
	~GfxDeviceTester();

	wg::Size	startWindowSize() override { return { 1400, 900 }; }


	// Pre-init configuration

	void	addTestDevice(Device* pDevice);
	void	destroy_testdevices();

	// Init/update/exit

	bool	init(AppVisitor* pVisitor) override;
	bool	update() override;
	void	exit() override;

	
protected:

	enum class DisplayMode
	{
		Testee,
		Reference,
		Both,
		Diff,
		Time
	};

	enum DeviceEnum
	{
		TESTEE = 0,
		REFERENCE = 1
	};

	enum class ClipList
	{
		One,
		Few,
		Many
	};


	struct SuiteEntry
	{
		bool		bActive = true;
		bool		bWorking;
		int			nbTests;
		TestSuite* pTesteeSuite;
		TestSuite* pRefSuite;
	};


	struct DeviceTest
	{
		Test* pTest = nullptr;
		double	render_time = 0;						// Seconds to render number of rounds
		double	stalling_time = 0;						// Seconds for endRender() call afterwards. OpenGL stalls here. 
	};

	struct TestEntry
	{
		string	name;
		bool	bActive = false;
		bool	bWorking;

		DeviceTest devices[2];							// Testee, followed by Reference
	};


	bool		setup_chrome();
	void		teardown_chrome();

	void		setup_tests();
	bool		add_testsuite(TestSuite* pTesteeSuite, TestSuite* pReferenceSuite);
	void		regen_testentries();

	void		run_tests(Device* pDevice, DeviceEnum device);
	void		clock_test(DeviceTest* pDeviceTest, int rounds, Device* pDevice);
	void		destroy_tests();

	void		update_displaymode();
	void		display_test_results();

	void		setup_cliplist(ClipList list);

	SurfaceDisplay_p	create_canvas();
	void		refresh_performance_display();
	void		refresh_performance_measurements();


	const SizeI			g_canvasSize = { 512, 512 };

	AppVisitor*			m_pVisitor = nullptr;

	ScrollPanel_p		g_pViewPanel = nullptr;

	Device* g_pTesteeDevice = nullptr;
	Device* g_pReferenceDevice = nullptr;

	SurfaceDisplay_p	g_pTesteeCanvas = nullptr;
	SurfaceDisplay_p	g_pReferenceCanvas = nullptr;

	Widget_p			g_pPerformanceDisplay = nullptr;
	PackList_p			g_pPerformanceList = nullptr;
	TextLayout_p		g_pPerformanceValueMapper = nullptr;
	SizeBroker_p		g_pPerformanceEntryBroker = nullptr;

	vector<Device*>		g_testdevices;
	vector<SuiteEntry>	g_testsuites;
	vector<TestEntry>	g_tests;

	vector<RectSPX>		g_clipList;

	TextStyle_p			g_pButtonLabelStyle = nullptr;
	TextLayout_p		g_pButtonLabelMapper = nullptr;

	DisplayMode			g_displayMode = DisplayMode::Testee;
	float				g_zoomFactor = 1.f;

	bool				g_bRedrawTestee = true;
	bool				g_bRedrawReference = true;
	bool				g_bRefreshPerformance = false;


};