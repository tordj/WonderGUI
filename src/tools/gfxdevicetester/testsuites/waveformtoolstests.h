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

	
		
		auto pFactory = pDevice->waveformFactory();
		
		m_pDonutWaveform = pFactory->createWaveform( WGBP(Waveform,
														 _.size = canvas.size()/64,
														 _.segments = 9,
														 _.colors =	m_outlinedDonutColors ));

		
		return true;
	}



	
	
	bool drawOutlinedDonut(GfxDevice * pDevice, const RectI& canvas)
	{
		WaveformTools::drawOutlinedDonut(m_pDonutWaveform, 20*64, 3*64 );
		pDevice->drawWaveform(canvas, m_pDonutWaveform );
		return true;
	}

	

private:

	Waveform_p	m_pDonutWaveform;
	
	HiColor	m_outlinedDonutColors[9] = { Color::Transparent, Color::White, Color::Blue, Color::White, Color::Transparent, Color::White, Color::Blue, Color::White, Color::Transparent };

	HiColor	m_rainbowColors[8] = { Color::Red, Color::Orange, Color::Yellow, Color::Green, Color::Blue, Color::Indigo, Color::Violet, Color( 255,255,255,128 ) };
};
 
