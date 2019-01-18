#include <testsuites/testsuite.h>

class LineTests : public TestSuite
{
public:
	LineTests()
	{
		name = "LineTests";

		addTest("SolarisLines", &LineTests::solarisLines );
		addTest("SolarisLinePatches", &LineTests::solarisLinesPatches );
	}

	bool init(GfxDevice * pDevice, const Rect& canvas)
	{
		const double PI = 3.1415;

		Coord center = { canvas.w / 2, canvas.h / 2 };

		for (int i = 0; i < c_nbSolarisLines; i++)
		{
			float xFactor = sin(i * PI * 2 / c_nbSolarisLines  );
			float yFactor = cos(i * PI * 2 / c_nbSolarisLines );

			Coord beg = center + Coord(50 * xFactor, 50 * yFactor);
			Coord end = center + Coord( 250 * xFactor, 250 * yFactor );

			m_begCoordsSolaris[i] = beg;
			m_endCoordsSolaris[i] = end;
		}

		Rect * wpPatches = m_patches;

		for( int y = 0 ; y < 512 ; y+= 128 )
		{
			for (int x = 0; x < 512; x += 128)
			{
				*wpPatches++ = { x, y, 64,64 };
				*wpPatches++ = { x+64, y+64, 64,64 };
			}
		}


		return true;
	}

	bool solarisLines(GfxDevice * pDevice, const Rect& canvas)
	{
		for (int i = 0; i < c_nbSolarisLines; i++)
		{
			pDevice->drawLine(m_begCoordsSolaris[i], m_endCoordsSolaris[i], Color::Yellow, 1.f);
		}

		return true;
	}

	bool solarisLinesPatches(GfxDevice * pDevice, const Rect& canvas)
	{
		pDevice->fillPatches(Color::Blue, c_nbPatches, m_patches);

		for (int i = 0; i < c_nbSolarisLines; i++)
		{
			pDevice->drawLinePatches(m_begCoordsSolaris[i], m_endCoordsSolaris[i], Color::Yellow, 1.f, c_nbPatches, m_patches );
		}
		return true;
	}


	const static int c_nbSolarisLines = 30;
	const static int c_nbPatches = 32;

	Coord	m_begCoordsSolaris[c_nbSolarisLines];
	Coord	m_endCoordsSolaris[c_nbSolarisLines];
	Rect	m_patches[c_nbPatches]; // = { {0,0,64,64}, {128,0,64,64}, {64,64,64,64}, {0,128,64,64}, {128,128,64,64} };

};
