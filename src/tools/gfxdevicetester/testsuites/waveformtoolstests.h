#include "testsuite.h"

#include <wg_waveformtools.h>

class WaveformToolsTests : public TestSuite
{
public:
	WaveformToolsTests()
	{
		name = "WaveformToolsTests";

		addTest("DrawOutlinedDonut", &WaveformToolsTests::drawOutlinedDonut );
	}


	bool init(GfxDevice * pDevice, const RectI& canvas, WonderApp::Visitor * pAppVisitor)
	{

		
		return true;
	}

	
	bool drawOutlinedDonut(GfxDevice * pDevice, const RectSPX& canvas)
	{
		auto pDonut = WaveformTools::createDonut(canvas.size(), 20*64, Color::Blue, 2*64, Color::White, pDevice->waveformFactory());
		pDevice->drawWaveform(canvas, pDonut );
		return true;
	}
	

private:

	HiColor	m_rainbowColors[8] = { Color::Red, Color::Orange, Color::Yellow, Color::Green, Color::Blue, Color::Indigo, Color::Violet, Color( 255,255,255,128 ) };
};
 
