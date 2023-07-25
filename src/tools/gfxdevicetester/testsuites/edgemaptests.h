#include "testsuite.h"

class EdgemapTests : public TestSuite
{
public:
	EdgemapTests()
	{
		name = "EdgemapTests";

		addTest("Edgemap SPX topdown", &EdgemapTests::spxTopDown );
		addTest("Edgemap SPX topdown flip 90", &EdgemapTests::spxTopDown90 );
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

				//m_edgemap1[wave][spl] = 10*(wave+1)*64;

				sampleBuffer[wave][spl] = (int)((ofs+sin(spl / 10.0) * (wave+1)*10) * 64);
			}
		}

		
		auto pFactory = pDevice->edgemapFactory();
		
		m_pEdgemapSPX1 = pFactory->createEdgemap( WGBP(Edgemap, _.size = canvas.size()/64, _.segments = 8, _.colors = m_rainbowColors ),
												  SampleOrigo::Top, sampleBuffer[0], 7 );

		
		return true;
	}



	bool spxTopDown(GfxDevice * pDevice, const RectI& canvas)
	{
		
		pDevice->drawEdgemap(canvas, m_pEdgemapSPX1 );
		return true;
	}

	bool spxTopDown90(GfxDevice * pDevice, const RectI& canvas)
	{
		pDevice->flipDrawEdgemap(canvas, m_pEdgemapSPX1, GfxFlip::Rot90 );
		return true;
	}

	

private:

	Edgemap_p	m_pEdgemapSPX1;
	

	HiColor	m_rainbowColors[8] = { Color::Red, Color::Orange, Color::Yellow, Color::Green, Color::Blue, Color::Indigo, Color::Violet, Color( 255,255,255,128 ) };
};
 
