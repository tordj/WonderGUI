#include <testsuites/testsuite.h>

class BlitConsistencyTests : public TestSuite
{
public:
	BlitConsistencyTests()
	{
		name = "BlitConsistencyTests";

		addTest("BlitXInter", &BlitConsistencyTests::setPaletteInterpolate, &BlitConsistencyTests::blitX, &BlitConsistencyTests::cleanup);
		addTest("BlitXNear", &BlitConsistencyTests::setPaletteNearest, &BlitConsistencyTests::blitX, &BlitConsistencyTests::cleanup);

		addTest("BlitPaletteInter", &BlitConsistencyTests::setPaletteInterpolate, &BlitConsistencyTests::blitPalette, &BlitConsistencyTests::cleanup);
		addTest("BlitPaletteNear", &BlitConsistencyTests::setPaletteNearest, &BlitConsistencyTests::blitPalette, &BlitConsistencyTests::cleanup);

		addTest("BlitFrameInter", &BlitConsistencyTests::setFrameInterpolate, &BlitConsistencyTests::blitFrame, &BlitConsistencyTests::cleanup);
		addTest("BlitFrameNear", &BlitConsistencyTests::setFrameNearest, &BlitConsistencyTests::blitFrame, &BlitConsistencyTests::cleanup);

		addTest("BlitInsideFrameInter", &BlitConsistencyTests::setFrameInterpolate, &BlitConsistencyTests::blitInsideFrame, &BlitConsistencyTests::cleanup);
		addTest("BlitInsideFrameNear", &BlitConsistencyTests::setFrameNearest, &BlitConsistencyTests::blitInsideFrame, &BlitConsistencyTests::cleanup);


	}

	bool init(GfxDevice * pDevice, const RectI& canvas)
	{
		m_pPalette = FileUtil::loadSurface("../resources/stretchblit_consistency.png", pDevice->surfaceFactory());
		if (!m_pPalette)
			return false;

		m_pFrame = FileUtil::loadSurface("../resources/white_frame_256x256.png", pDevice->surfaceFactory());
		if (!m_pFrame)
			return false;
		
		return true;
	}
	 
	bool exit(GfxDevice * pDevice, const RectI& canvas)
	{
		m_pPalette = nullptr;
		m_pFrame = nullptr;
		return true;
	}


	bool cleanup(GfxDevice * pDevice, const RectI& canvas)
	{
		pDevice->setBlitSource(nullptr);
		return true;
	}

	bool setPaletteInterpolate(GfxDevice * pDevice, const RectI& canvas)
	{
		m_pPalette->setScaleMode(ScaleMode::Interpolate);
		pDevice->setBlitSource(m_pPalette);
		return true;
	}

	bool setPaletteNearest(GfxDevice * pDevice, const RectI& canvas)
	{
		m_pPalette->setScaleMode(ScaleMode::Nearest);
		pDevice->setBlitSource(m_pPalette);
		return true;
	}

	bool setFrameInterpolate(GfxDevice * pDevice, const RectI& canvas)
	{
		m_pFrame->setScaleMode(ScaleMode::Interpolate);
		pDevice->setBlitSource(m_pFrame);
		return true;
	}

	bool setFrameNearest(GfxDevice * pDevice, const RectI& canvas)
	{
		m_pFrame->setScaleMode(ScaleMode::Nearest);
		pDevice->setBlitSource(m_pFrame);
		return true;
	}


	bool	blitPalette(GfxDevice * pDevice, const RectI& canvas)
	{
		pDevice->stretchBlit({ 64,0,190,64 }, RectI( 1,1,3,3 ) );
		pDevice->stretchBlit({ 0,64,64,190 }, RectI( 5,1,3,3 ) );
		pDevice->stretchBlit({ 1,1,60,60 }, RectI(1,5,1,1) );
		return true;
	}

	bool	blitFrame(GfxDevice * pDevice, const RectI& canvas)
	{
		for( int i = 0 ; i < 82 ; i++ )
			pDevice->stretchBlit({ i*3,i*3,512-i*6,512-i*6 }, RectI(0, 0, 256, 256));

		return true;
	}

	bool	blitInsideFrame(GfxDevice * pDevice, const RectI& canvas)
	{
		for (int i = 0; i < 82 ; i++)
			pDevice->stretchBlit({ i * 3,i * 3,512 - i * 6,512 - i * 6 }, RectI(1, 1, 254, 254));

		return true;
	}

	bool	blitX(GfxDevice * pDevice, const RectI& canvas)
	{
		pDevice->stretchBlit({ 10,10,3,3 }, RectI(1, 1, 3, 3));
		return true;
	}



private:
	Surface_p	m_pPalette;
	Surface_p	m_pFrame;
};
