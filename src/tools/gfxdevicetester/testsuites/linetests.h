#include "testsuite.h"

class LineTests : public TestSuite
{
public:
	LineTests()
	{
		name = "LineTests";

		addTest("SolarisLines", &LineTests::solarisLines );
	}

	bool init(GfxDevice * pDevice, const RectI& canvas, AppVisitor * pAppVisitor)
	{
		const double PI = 3.1415;

		CoordI center = { canvas.w / 2, canvas.h / 2 };

		for (int i = 0; i < c_nbSolarisLines; i++)
		{
			float xFactor = (float) sin(i * PI * 2 / c_nbSolarisLines  );
			float yFactor = (float) cos(i * PI * 2 / c_nbSolarisLines );

			CoordI beg = center + CoordI(int(50 * xFactor), int(50 * yFactor) );
			CoordI end = center + CoordI( int(250 * xFactor), int(250 * yFactor) );

			m_begCoordsSolaris[i] = beg;
			m_endCoordsSolaris[i] = end;
		}

		return true;
	}

	

	bool solarisLines(GfxDevice * pDevice, const RectI& canvas)
	{
		for (int i = 0; i < c_nbSolarisLines; i++)
		{
			pDevice->drawLine(m_begCoordsSolaris[i], m_endCoordsSolaris[i], Color::Yellow, 1.f);
		}

		return true;
	}


	const static int c_nbSolarisLines = 30;

	CoordI	m_begCoordsSolaris[c_nbSolarisLines];
	CoordI	m_endCoordsSolaris[c_nbSolarisLines];

};
