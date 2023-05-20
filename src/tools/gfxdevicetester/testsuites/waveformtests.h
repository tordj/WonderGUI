#include "testsuite.h"

class WaveformTests : public TestSuite
{
public:
	WaveformTests()
	{
		name = "WaveformTests";

		addTest("Waveform SPX topdown", &WaveformTests::spxTopDown );
	}


	bool init(GfxDevice * pDevice, const RectI& canvas, WonderApp::Visitor * pAppVisitor)
	{
		int ofs = 20;

		for (int wave = 0; wave < 7; wave++)
		{
			ofs += (wave+1)*12;
			
			for (int spl = 0; spl < 512+1; spl++)
			{

				//m_waveform1[wave][spl] = 10*(wave+1)*64;

				m_waveform1[wave][spl] = (int)((ofs+sin(spl / 10.0) * (wave+1)*10) * 64);
			}
		}
		
		return true;
	}



	bool spxTopDown(GfxDevice * pDevice, const RectI& canvas)
	{
		
		auto pFactory = pDevice->waveformFactory();
		
		auto pWaveform = pFactory->createWaveform( WGBP(Waveform, _.size = canvas.size()/64, _.segments = 8, _.colors = m_rainbowColors ),
												  WaveOrigo::Top, m_waveform1[0], 7 );

		pDevice->drawWaveform(canvas, pWaveform );
		return true;
	}



private:


	spx m_waveform1[7][512+1];
	

	HiColor	m_rainbowColors[8] = { Color::Red, Color::Orange, Color::Yellow, Color::Green, Color::Blue, Color::Indigo, Color::Violet, Color( 255,255,255,128 ) };
};
 
