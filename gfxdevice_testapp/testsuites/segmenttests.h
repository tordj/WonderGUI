#include <testsuites/testsuite.h>

class SegmentTests : public TestSuite
{
public:
	SegmentTests()
	{
		name = "SegmentTests";

		addTest("RainbowSpread", &SegmentTests::rainbowSpread );
		addTest("WildRainbow", &SegmentTests::wildRainbow);
		addTest("Circles", &SegmentTests::circles);
		addTest("Elipses", &SegmentTests::elipses);
		addTest("LargeCircle", &SegmentTests::largeCircle);
		addTest("BrokenElipse", &SegmentTests::brokenElipse);
	}


	bool rainbowSpread(GfxDevice * pDevice, const Rect& canvas)
	{
		Color colors[8] = { Color::Red, Color::Orange, Color::Yellow, Color::Green, Color::Blue, Color::Indigo, Color::Violet, { 255,255,255,128 } };

		int edges[512+1][7];

		for (int i = 0; i <= canvas.w; i++)
		{
			edges[i][0] = i * 32;
			edges[i][1] = i * 64;
			edges[i][2] = i * 96;
			edges[i][3] = i * 128;
			edges[i][4] = i * 160;
			edges[i][5] = i * 192;
			edges[i][6] = i * 224;
		}

		pDevice->drawSegments(canvas, 8, colors, &edges[0][0], 7);

		return true;
	}

	bool wildRainbow(GfxDevice * pDevice, const Rect& canvas)
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

		pDevice->drawSegments(canvas, 8, colors, &edges[0][0], 7);

		return true;
	}

	bool largeCircle(GfxDevice * pDevice, const Rect& canvas)
	{
		pDevice->drawElipse(canvas, 20.f, Color::Aquamarine, 2.f, Color::White);
		return true;
	}

	bool circles(GfxDevice * pDevice, const Rect& canvas)
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

	bool elipses(GfxDevice * pDevice, const Rect& canvas)
	{
		pDevice->drawElipse({ 0.f,0.f,canvas.w*3/4.f, canvas.w*3/4/3.f}, 20.f, Color::Aquamarine, 5.f, Color::White);
		pDevice->drawElipse({ canvas.w*3/4.f,0.f,canvas.w/4.f, canvas.h*1.f }, 20.f, Color::Aquamarine, 5.f, Color::White);

		return true;
	}


	bool brokenElipse(GfxDevice * pDevice, const Rect& canvas)
	{
		int x = 0;
		int y = 1;

		pDevice->drawElipse({ x*30.1f,y * 20 + y * y*1.5f + x * 0.11f,y*15.01f + x * 2.05f,y*10.01f + x * 2.05f }, 4.f, Color::Aquamarine, 1.f, Color::White);

		return true;
	}


};
 