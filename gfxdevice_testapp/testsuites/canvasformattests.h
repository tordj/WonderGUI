#include <testsuites/testsuite.h>

class CanvasFormatTests : public TestSuite
{
public:
	CanvasFormatTests()
	{
		name = "CanvasFormatTests";

		addTest("DrawToBGR_8", &CanvasFormatTests::init_BGR_8, &CanvasFormatTests::drawPrimitives, &CanvasFormatTests::exit_drawToSurface );
		addTest("DrawToBGRA_8", &CanvasFormatTests::init_BGRA_8, &CanvasFormatTests::drawPrimitives, &CanvasFormatTests::exit_drawToSurface);
		addTest("DrawToBGRX_8", &CanvasFormatTests::init_BGRX_8, &CanvasFormatTests::drawPrimitives, &CanvasFormatTests::exit_drawToSurface);
		addTest("DrawToBGRA_4", &CanvasFormatTests::init_BGRA_4, &CanvasFormatTests::drawPrimitives, &CanvasFormatTests::exit_drawToSurface);
		addTest("DrawToBGR_565", &CanvasFormatTests::init_BGR_565, &CanvasFormatTests::drawPrimitives, &CanvasFormatTests::exit_drawToSurface);
	}

	bool init(GfxDevice * pDevice, const RectI& canvas)
	{
		m_pCanvasBGR_8 = pDevice->surfaceFactory()->createSurface(canvas, PixelFormat::BGR_8);
		m_pCanvasBGRA_8 = pDevice->surfaceFactory()->createSurface(canvas, PixelFormat::BGRA_8);
		m_pCanvasBGRX_8 = pDevice->surfaceFactory()->createSurface(canvas, PixelFormat::BGRX_8);
		m_pCanvasBGRA_4 = pDevice->surfaceFactory()->createSurface(canvas, PixelFormat::BGRA_4);
		m_pCanvasBGR_565 = pDevice->surfaceFactory()->createSurface(canvas, PixelFormat::BGR_565);

		return true;
	}

	bool	init_BGR_8(GfxDevice * pDevice, const RectI& canvas)
	{
		m_pActiveCanvas = m_pCanvasBGR_8;
		return init_drawToSurface(pDevice, canvas);
	}

	bool	init_BGRA_8(GfxDevice * pDevice, const RectI& canvas)
	{
		m_pActiveCanvas = m_pCanvasBGRA_8;
		return init_drawToSurface(pDevice, canvas);
	}

	bool	init_BGRX_8(GfxDevice * pDevice, const RectI& canvas)
	{
		m_pActiveCanvas = m_pCanvasBGRX_8;
		return init_drawToSurface(pDevice, canvas);
	}

	bool	init_BGRA_4(GfxDevice * pDevice, const RectI& canvas)
	{
		m_pActiveCanvas = m_pCanvasBGRA_4;
		return init_drawToSurface(pDevice, canvas);
	}

	bool	init_BGR_565(GfxDevice * pDevice, const RectI& canvas)
	{
		m_pActiveCanvas = m_pCanvasBGR_565;
		return init_drawToSurface(pDevice, canvas);
	}

	bool	init_drawToSurface(GfxDevice * pDevice, const RectI& canvas)
	{
		m_pActiveCanvas->fill(Color::Transparent);
		m_pSavedCanvas = pDevice->canvas();
		m_pSavedClipList = pDevice->clipList();
		m_savedClipListSize = pDevice->clipListSize();
		pDevice->setCanvas(m_pActiveCanvas);
		return true;
	}

	bool	exit_drawToSurface(GfxDevice * pDevice, const RectI& canvas)
	{
		pDevice->setCanvas(m_pSavedCanvas);
		pDevice->setClipList(m_savedClipListSize, m_pSavedClipList);
		pDevice->setBlitSource(m_pActiveCanvas);
		pDevice->blit({ 0,0 });
		m_pActiveCanvas = nullptr;
		return true;
	}


	bool	drawPrimitives(GfxDevice * pDevice, const RectI& canvas)
	{
		pDevice->drawLine(canvas.pos() + CoordI(10, 10), canvas.pos() + CoordI(canvas.size().w, canvas.size().h) - CoordI(10, 20), Color::Red, 3.f);
		pDevice->drawLine(canvas.pos() + CoordI(10, 20), canvas.pos() + CoordI(canvas.size().w, canvas.size().h) - CoordI(10, 10), Color(0, 0, 255, 128), 3.f);

		pDevice->drawLine(canvas.pos() + CoordI(5, 100), canvas.pos() + CoordI(40,101), Color::Green, 3.f);
		pDevice->drawLine(canvas.pos() + CoordI(5, 105), canvas.pos() + CoordI(6, 145), Color::Green, 3.f);


		CoordI	fillOfs = { canvas.x, canvas.y + canvas.h / 2 };
		SizeI	fillSize = { 50,50 };
		CoordI	stepping = { 60, 0 };

		pDevice->fill({ fillOfs, fillSize }, Color::Red);
		pDevice->fill({ fillOfs + stepping, fillSize }, Color(0, 0, 255, 128));
		pDevice->fill({ fillOfs + stepping * 2, fillSize }, Color(0, 0, 255, 64));
		pDevice->fill({ fillOfs + stepping * 3, fillSize }, Color(0, 0, 255, 32));
		pDevice->fill({ fillOfs + stepping * 4, fillSize }, Color(0, 0, 255, 16));

		return true;
	}



private:

	Surface_p		m_pActiveCanvas;
	Surface_p		m_pSavedCanvas;
	const RectI *	m_pSavedClipList;
	int				m_savedClipListSize;

	Surface_p		m_pCanvasBGR_8;
	Surface_p		m_pCanvasBGRA_8;
	Surface_p		m_pCanvasBGRX_8;
	Surface_p		m_pCanvasBGRA_4;
	Surface_p		m_pCanvasBGR_565;
};
