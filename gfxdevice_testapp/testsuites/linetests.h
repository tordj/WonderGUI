#include <testsuites/testsuite.h>

class LineTests : public TestSuite
{
public:
	LineTests()
	{
		name = "LineTests";

		addTest("SolarisLines", &LineTests::solarisLines );
	}

	bool init(GfxDevice * pDevice, const Rect& canvas)
	{
		const double PI = 3.1415;

		Coord center = { canvas.w / 2, canvas.h / 2 };

		for (int i = 0; i < c_nbSolarisLines; i++)
		{
			float xFactor = (float) sin(i * PI * 2 / c_nbSolarisLines  );
			float yFactor = (float) cos(i * PI * 2 / c_nbSolarisLines );

			Coord beg = center + Coord(int(50 * xFactor), int(50 * yFactor) );
			Coord end = center + Coord( int(250 * xFactor), int(250 * yFactor) );

			m_begCoordsSolaris[i] = beg;
			m_endCoordsSolaris[i] = end;
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


	const static int c_nbSolarisLines = 30;

	Coord	m_begCoordsSolaris[c_nbSolarisLines];
	Coord	m_endCoordsSolaris[c_nbSolarisLines];

};
