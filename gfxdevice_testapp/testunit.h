
#include <string>
#include <wondergui.h>
#include "wg_fileutil.h"

using namespace wg;
using namespace std;


class TestUnit
{
public:
	TestUnit() {}
	virtual ~TestUnit() {}

	virtual const string	name() const = 0;
	virtual bool			init(GfxDevice * pDevice, const Rect& canvas ) { return true;  }
	virtual bool			run(GfxDevice * pDevice, const Rect& canvas) = 0;

	bool		bSelected = false;



};


namespace test
{

	class StraightFill : public TestUnit
	{
	public:

		const string name() const { return "StraightFill"; }

		bool run(GfxDevice * pDevice, const Rect& canvas)
		{
			pDevice->setBlendMode(BlendMode::Replace);
			pDevice->fill( canvas, Color::RosyBrown );
			pDevice->setBlendMode(BlendMode::Blend);

			return true;
		}

	};

	class BlendFill : public TestUnit
	{
	public:

		const string name() const { return "BlendFill"; }

		bool run(GfxDevice * pDevice, const Rect& canvas)
		{
			pDevice->setTintColor({ 255,255,255,128 });

			pDevice->fill( canvas, Color::RosyBrown );

			pDevice->setTintColor({ 255,255,255,255 });

			return true;
		}

	};


	class OffscreenBGRACanvas : public TestUnit
	{
	public:
		const string	name() const { return "OffscreenBGRACanvas";  }
		bool			init(GfxDevice * pDevice, const Rect& canvas )
		{
			m_pMyCanvas = pDevice->surfaceFactory()->createSurface(canvas, PixelFormat::BGRA_8);

			m_pBlitSource = FileUtil::loadSurface("../resources/splash.png", pDevice->surfaceFactory());
			return(m_pMyCanvas != nullptr && m_pBlitSource != nullptr);
		}

		bool			run(GfxDevice * pDevice, const Rect& canvas)
		{

			m_pMyCanvas->fill(Color::Transparent);

			pDevice->fill(canvas.size(), Color(0,255,0));
			Surface_p pOldCanvas = pDevice->canvas();

			pDevice->setCanvas(m_pMyCanvas);

			pDevice->clipBlit(canvas, m_pBlitSource, Coord(50, 50));

			pDevice->clipDrawLine(canvas, canvas.pos() + Coord(10, 10), canvas.pos() + Coord(canvas.size().w, canvas.size().h) - Coord(10, 20), Color::Red, 3.f);
			pDevice->clipDrawLine(canvas, canvas.pos() + Coord(10, 20), canvas.pos() + Coord(canvas.size().w, canvas.size().h) - Coord(10, 10), Color(0,0,255,128), 3.f);

			Coord	fillOfs = { canvas.x, canvas.y + canvas.h / 2 };
			Size	fillSize = { 50,50 };
			Coord	stepping = { 60, 0 };

			pDevice->clipFill(canvas, { fillOfs, fillSize }, Color::Red);
			pDevice->clipFill(canvas, { fillOfs + stepping, fillSize }, Color(0, 0, 255, 128));
			pDevice->clipFill(canvas, { fillOfs + stepping*2, fillSize }, Color(0, 0, 255, 64));
			pDevice->clipFill(canvas, { fillOfs + stepping * 3, fillSize }, Color(0, 0, 255, 32));
			pDevice->clipFill(canvas, { fillOfs + stepping * 4, fillSize }, Color(0, 0, 255, 16));


			pDevice->setCanvas(pOldCanvas);
			pDevice->clipBlit(canvas, m_pMyCanvas);

			return true;
		}

	protected:

		Surface_p	m_pMyCanvas;
		Surface_p	m_pBlitSource;


	};

	class StretchBlitBlends : public TestUnit
	{
	public:
		const string	name() const { return "StretchBlitBlends"; }
		bool			init(GfxDevice * pDevice, const Rect& canvas)
		{
			m_pMyCanvas = pDevice->surfaceFactory()->createSurface(canvas, PixelFormat::BGRA_8);

			m_pBlitSource = FileUtil::loadSurface("../resources/splash.png", pDevice->surfaceFactory());
			return(m_pMyCanvas != nullptr && m_pBlitSource != nullptr);
		}

		bool			run(GfxDevice * pDevice, const Rect& canvas)
		{

//			m_pMyCanvas->fill(Color::Transparent);

//			pDevice->fill(canvas.size(), Color(0, 255, 0));
//			Surface_p pOldCanvas = pDevice->canvas();

//			pDevice->setCanvas(m_pMyCanvas);

			pDevice->clipStretchBlit(canvas, m_pBlitSource, Rect(50, 50,256,128));
			pDevice->clipStretchBlit(canvas, m_pBlitSource, Rect(0, 0, 128, 64));
			pDevice->clipStretchBlit(canvas, m_pBlitSource, Rect(0, 256, 400, 140));

//			pDevice->setCanvas(pOldCanvas);
//			pDevice->clipBlit(canvas, m_pMyCanvas);

			return true;
		}

	protected:

		Surface_p	m_pMyCanvas;
		Surface_p	m_pBlitSource;


	};



}