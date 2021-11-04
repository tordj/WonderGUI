#include "testsuite.h"

class PlotTests : public TestSuite
{
public:
	PlotTests()
	{
		name = "PlotTests";

		addTest("Plot", &PlotTests::plot );
	}

	bool init(GfxDevice * pDevice, const RectSPX& canvas, AppVisitor * pAppVisitor)
	{
		m_coords[0] = { 1 * 64,1 * 64 };
		m_coords[1] = { 3 * 64,1 * 64 };
		m_coords[2] = { 1 * 64,3 * 64 };
		m_coords[3] = { 3 * 64,3 * 64 };

		m_colors[0] = Color::Red;
		m_colors[1] = Color::Green;
		m_colors[2] = Color::Blue;
		m_colors[3] = Color::White;

		m_coords[4] = { 1 * 64,5 * 64 };
		m_coords[5] = { 1 * 64,7 * 64 };
		m_coords[6] = { 1 * 64,9 * 64 };
		m_coords[7] = { 1 * 64,11 * 64 };

		m_colors[4] = Color(255,0,0,128);
		m_colors[5] = Color(255,0,0,64);
		m_colors[6] = Color(255,0,0,32);
		m_colors[7] = Color(255,0,0,16);

		for (int i = 8; i < c_nbCoords; i++)
		{
			m_coords[i] = { rand() % canvas.w, rand() % canvas.h };
			m_colors[i] = Color::Beige;
		}

		return true;
	}


	bool plot(GfxDevice * pDevice, const RectSPX& canvas)
	{
		pDevice->plotPixels( c_nbCoords, m_coords, m_colors );
		return true;
	}

	const static int c_nbCoords = 1000;

	CoordI	m_coords[c_nbCoords];
	HiColor	m_colors[c_nbCoords];

};
