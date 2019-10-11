#include <testsuites/testsuite.h>

class SegmentTests : public TestSuite
{
public:
	SegmentTests()
	{
		name = "SegmentTests";

		addTest("RainbowSpread7", &SegmentTests::rainbowSpread7);
		addTest("RainbowSpread8", &SegmentTests::rainbowSpread8 );
		addTest("Flip90Rainbow", &SegmentTests::flip90Rainbow);
		addTest("Flip180Rainbow", &SegmentTests::flip180Rainbow);
		addTest("Flip270Rainbow", &SegmentTests::flip270Rainbow);
		addTest("WildRainbow", &SegmentTests::wildRainbow);
		addTest("Circles", &SegmentTests::circles);
		addTest("Elipses", &SegmentTests::elipses);
		addTest("LargeCircle", &SegmentTests::largeCircle);
		addTest("BrokenElipse", &SegmentTests::brokenElipse);

		addTest("SegBlendReplace", &SegmentTests::setBlendModeReplace, &SegmentTests::rainbowSpread8, &SegmentTests::resetBlendMode);
		addTest("SegBlendAdd", &SegmentTests::setBlendModeAdd, &SegmentTests::rainbowSpread8, &SegmentTests::resetBlendMode);
		addTest("SegBlendSubtract", &SegmentTests::setBlendModeSubtract, &SegmentTests::rainbowSpread8, &SegmentTests::resetBlendMode);
		addTest("SegBlendMultiply", &SegmentTests::setBlendModeMultiply, &SegmentTests::rainbowSpread8, &SegmentTests::resetBlendMode);
		addTest("SegBlendInvert", &SegmentTests::setBlendModeInvert, &SegmentTests::rainbowSpread8, &SegmentTests::resetBlendMode);

		addTest("SegPrecision", &SegmentTests::precisionTest);

	}


	bool init(GfxDevice * pDevice, const RectI& canvas)
	{
		// Generate rainbow segments

		for (int i = 0; i <= canvas.w; i++)
		{
			m_rainbowEdges[i][0] = i * 32;
			m_rainbowEdges[i][1] = i * 64;
			m_rainbowEdges[i][2] = i * 96;
			m_rainbowEdges[i][3] = i * 128;
			m_rainbowEdges[i][4] = i * 160;
			m_rainbowEdges[i][5] = i * 192;
			m_rainbowEdges[i][6] = i * 224;
		}

		return true;
	}


	bool	resetBlendMode(GfxDevice * pDevice, const RectI& canvas)
	{
		return pDevice->setBlendMode(BlendMode::Blend);
	}

	bool	setBlendModeReplace(GfxDevice * pDevice, const RectI& canvas)
	{
		return pDevice->setBlendMode(BlendMode::Replace);
	}

	bool	setBlendModeAdd(GfxDevice * pDevice, const RectI& canvas)
	{
		return pDevice->setBlendMode(BlendMode::Add);
	}

	bool	setBlendModeSubtract(GfxDevice * pDevice, const RectI& canvas)
	{
		return pDevice->setBlendMode(BlendMode::Subtract);
	}

	bool	setBlendModeMultiply(GfxDevice * pDevice, const RectI& canvas)
	{
		return pDevice->setBlendMode(BlendMode::Multiply);
	}

	bool	setBlendModeInvert(GfxDevice * pDevice, const RectI& canvas)
	{
		return pDevice->setBlendMode(BlendMode::Invert);
	}

	bool rainbowSpread7(GfxDevice * pDevice, const RectI& canvas)
	{

		pDevice->drawSegments(canvas, 7, m_rainbowColors, m_nRainbowEdges, &m_rainbowEdges[0][0], 7);

		return true;
	}

	bool rainbowSpread8(GfxDevice * pDevice, const RectI& canvas)
	{

		pDevice->drawSegments(canvas, 8, m_rainbowColors, m_nRainbowEdges, &m_rainbowEdges[0][0], 7);

		return true;
	}


	bool wildRainbow(GfxDevice * pDevice, const RectI& canvas)
	{
		Color colors[8] = { Color::Red, Color::Orange, Color::Yellow, Color::Green, Color::Blue, Color::Indigo, Color::Violet, { 0,0,0,255 } };

		int edges[512 + 1][7];
		for (int i = 0; i <= canvas.w; i++)
		{

			if (i % 2 == 0)
			{
				for (int j = 0; j < 7; j++)
					edges[i][j] = 1024 + 1024 * j;
			}
			else
			{
				for (int j = 0; j < 7; j++)
					edges[i][j] = (canvas.h*256) + 1024 * j;
			}
		}

		pDevice->drawSegments(canvas, 8, colors, m_nRainbowEdges, &edges[0][0], 7);

		return true;
	}

	bool largeCircle(GfxDevice * pDevice, const RectI& canvas)
	{
		pDevice->drawElipse(static_cast<RectF>(canvas), 20.f, Color::Aquamarine, 2.f, Color::White);
		return true;
	}

	bool circles(GfxDevice * pDevice, const RectI& canvas)
	{
		for (int y = 0; y < 10; y++)
		{
			for (int x = 0; x < 12; x++)
			{
				pDevice->drawElipse({ x*32.1f,y*26+y*y*1.5f+x*0.11f,y*15.01f+x*2.05f,y*15.01f+x*2.05f }, 4.f, Color::Aquamarine, 1.f, Color::White);
			}
		}

		return true;
	}

	bool elipses(GfxDevice * pDevice, const RectI& canvas)
	{
		pDevice->drawElipse({ 0.f,0.f,canvas.w*3/4.f, canvas.w*3/4/3.f}, 20.f, Color::Aquamarine, 5.f, Color::White);
		pDevice->drawElipse({ canvas.w*3/4.f,0.f,canvas.w/4.f, canvas.h*1.f }, 20.f, Color::Aquamarine, 5.f, Color::White);

		return true;
	}


	bool brokenElipse(GfxDevice * pDevice, const RectI& canvas)
	{
		int x = 0;
		int y = 1;

		pDevice->drawElipse({ x*30.1f,y * 20 + y * y*1.5f + x * 0.11f,y*15.01f + x * 2.05f,y*10.01f + x * 2.05f }, 4.f, Color::Aquamarine, 1.f, Color::White);

		return true;
	}


	bool rainbow(GfxDevice * pDevice, const RectI& canvas)
	{
		pDevice->drawSegments(canvas, 8, m_rainbowColors, m_nRainbowEdges, (int*)m_rainbowEdges, 7);
		return true;
	}


	bool flip90Rainbow(GfxDevice * pDevice, const RectI& canvas)
	{
		pDevice->flipDrawSegments(canvas, 8, m_rainbowColors, m_nRainbowEdges, (int*)m_rainbowEdges, 7, GfxFlip::Rot90);
		return true;
	}

	bool flip180Rainbow(GfxDevice * pDevice, const RectI& canvas)
	{
		pDevice->flipDrawSegments(canvas, 8, m_rainbowColors, m_nRainbowEdges, (int*)m_rainbowEdges, 7, GfxFlip::Rot180);
		return true;
	}

	bool flip270Rainbow(GfxDevice * pDevice, const RectI& canvas)
	{
		pDevice->flipDrawSegments(canvas, 8, m_rainbowColors, m_nRainbowEdges, (int*)m_rainbowEdges, 7, GfxFlip::Rot270);
		return true;
	}

	bool precisionTest(GfxDevice * pDevice, const RectI& canvas)
	{
		pDevice->plotPixels(4, m_precisionPlotCoords, m_precisionPlotColors);

//		pDevice->drawSegments({ 2,1,1,4 }, 5, m_precisionColors, 2, &m_precisionEdges[0][0], 4);
//		pDevice->drawSegments({ canvas.w-1,1,1,4 }, 5, m_precisionColors, 2, &m_precisionEdges[0][0], 4);

		pDevice->drawSegments({ 0,50,1,340 }, 5, m_precisionColors, 2, &m_precisionEdges2[0][0], 4);
		pDevice->drawSegments({ canvas.w-2,50,1,340 }, 5, m_precisionColors, 2, &m_precisionEdges2[0][0], 4);

		return true;
	}


private:

	Color	m_rainbowColors[8] = { Color::Red, Color::Orange, Color::Yellow, Color::Green, Color::Blue, Color::Indigo, Color::Violet, { 255,255,255,128 } };

	int		m_nRainbowEdges = 512 + 1;
	int		m_rainbowEdges[512 + 1][7];

	CoordI	m_precisionPlotCoords[4] = { {1,1}, {1,2}, {1,3}, {1,4} };
	Color	m_precisionPlotColors[4] = { {42,42,42,255}, {128,128,128,255}, {213,213,213,255}, {255,255,255,255} };

	int		m_precisionEdges[2][4] = { {0 * 256+128, 6 * 256, 6 * 256, 6 * 256}, {3 * 256+128, 6 * 256, 6 * 256, 6 * 256} };
	Color	m_precisionColors[5] = { {0,0,0,0}, {255,255,255,255}, {255,0,0,255}, {255,0,0,255}, {255,0,0,255} };

	int		m_precisionEdges2[2][4] = { {200 * 256 + 240, 219 * 256 + 89, 260 * 256, 260 * 256}, {206 * 256+170, 240 * 256, 260 * 256, 260 * 256} };


};

