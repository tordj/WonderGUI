#include "testsuites/testsuite.h"

class PlotTests : public TestSuite
{
public:
	PlotTests()
	{
		name = "PlotTests";

		addTest("Plot", &PlotTests::plot );
	}

	bool init(GfxDevice * pDevice, const RectI& canvas, AppVisitor * pAppVisitor)
	{
		m_coords[0] = { 1,1 };
		m_coords[1] = { 3,1 };
		m_coords[2] = { 1,3 };
		m_coords[3] = { 3,3 };

		m_colors[0] = HiColor::Red;
		m_colors[1] = HiColor::Green;
		m_colors[2] = HiColor::Blue;
		m_colors[3] = HiColor::White;

		m_coords[4] = { 1,5 };
		m_coords[5] = { 1,7 };
		m_coords[6] = { 1,9 };
		m_coords[7] = { 1,11 };

		m_colors[4] = { 4096,0,0,2048 };
		m_colors[5] = { 4096,0,0,1024 };
		m_colors[6] = { 4096,0,0,512 };
		m_colors[7] = { 4096,0,0,256 };

		for (int i = 8; i < c_nbCoords; i++)
		{
			m_coords[i] = { rand() % canvas.w, rand() % canvas.h };
			m_colors[i] = HiColor::Beige;
		}

		return true;
	}


	bool plot(GfxDevice * pDevice, const RectI& canvas)
	{
		pDevice->plotPixels( c_nbCoords, m_coords, m_colors );
		return true;
	}

	const static int c_nbCoords = 1000;

	CoordI	m_coords[c_nbCoords];
	HiColor	m_colors[c_nbCoords];

};
