#include "testsuite.h"

class WaveformTests : public TestSuite
{
public:
	WaveformTests()
	{
		name = "WaveformTests";

		addTest("Waveform SPX topdown", &WaveformTests::spxTopDown );
		addTest("Waveform SPX topdown flip 90", &WaveformTests::spxTopDown90 );
	}


	bool init(GfxDevice * pDevice, const RectI& canvas, WonderApp::Visitor * pAppVisitor)
	{

		spx sampleBuffer[7][512+1];
		
		int ofs = 20;

		for (int wave = 0; wave < 7; wave++)
		{
			ofs += (wave+1)*12;
			
			for (int spl = 0; spl < 512+1; spl++)
			{

				//m_waveform1[wave][spl] = 10*(wave+1)*64;

				sampleBuffer[wave][spl] = (int)((ofs+sin(spl / 10.0) * (wave+1)*10) * 64);
			}
		}

		
		auto pFactory = pDevice->waveformFactory();
		
		m_pWaveformSPX1 = pFactory->createWaveform( WGBP(Waveform, _.size = canvas.size()/64, _.segments = 8, _.colors = m_rainbowColors ),
												  WaveOrigo::Top, sampleBuffer[0], 7 );

		
		return true;
	}



	bool spxTopDown(GfxDevice * pDevice, const RectI& canvas)
	{
		
		pDevice->drawWaveform(canvas, m_pWaveformSPX1 );
		return true;
	}

	bool spxTopDown90(GfxDevice * pDevice, const RectI& canvas)
	{
		pDevice->flipDrawWaveform(canvas, m_pWaveformSPX1, GfxFlip::Rot90 );
		return true;
	}

	

private:

	Waveform_p	m_pWaveformSPX1;
	

	HiColor	m_rainbowColors[8] = { Color::Red, Color::Orange, Color::Yellow, Color::Green, Color::Blue, Color::Indigo, Color::Violet, Color( 255,255,255,128 ) };
};
 
