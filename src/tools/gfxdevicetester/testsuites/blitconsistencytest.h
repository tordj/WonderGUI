#include "testsuite.h"

class BlitConsistencyTests : public TestSuite
{
public:
	BlitConsistencyTests()
	{
		name = "BlitConsistencyTests";

		addTest("Blit3x3Inter", &BlitConsistencyTests::setPaletteInterpolate, &BlitConsistencyTests::blitX, &BlitConsistencyTests::cleanup);
		addTest("Blit3x3Near", &BlitConsistencyTests::setPaletteNearest, &BlitConsistencyTests::blitX, &BlitConsistencyTests::cleanup);

		addTest("BlitPaletteInter", &BlitConsistencyTests::setPaletteInterpolate, &BlitConsistencyTests::blitPalette, &BlitConsistencyTests::cleanup);
		addTest("BlitPaletteNear", &BlitConsistencyTests::setPaletteNearest, &BlitConsistencyTests::blitPalette, &BlitConsistencyTests::cleanup);

		addTest("BlitFrameInter", &BlitConsistencyTests::setFrameInterpolate, &BlitConsistencyTests::blitFrame, &BlitConsistencyTests::cleanup);
		addTest("BlitFrameNear", &BlitConsistencyTests::setFrameNearest, &BlitConsistencyTests::blitFrame, &BlitConsistencyTests::cleanup);

		addTest("BlitInsideFrameInter", &BlitConsistencyTests::setFrameInterpolate, &BlitConsistencyTests::blitInsideFrame, &BlitConsistencyTests::cleanup);
		addTest("BlitInsideFrameNear", &BlitConsistencyTests::setFrameNearest, &BlitConsistencyTests::blitInsideFrame, &BlitConsistencyTests::cleanup);

        addTest("BlitGridInter", &BlitConsistencyTests::setGridInterpolate, &BlitConsistencyTests::blitGrid, &BlitConsistencyTests::cleanup);
        addTest("BlitGridNear", &BlitConsistencyTests::setGridNearest, &BlitConsistencyTests::blitGrid, &BlitConsistencyTests::cleanup);

	}

	bool init(GfxDevice * pDevice, const RectI& canvas, AppVisitor * pAppVisitor)
	{
		m_pPalette = pAppVisitor->loadSurface("resources/stretchblit_consistency.png", pDevice->surfaceFactory());
		if (!m_pPalette)
			return false;

		m_pFrame = pAppVisitor->loadSurface("resources/white_frame_256x256.png", pDevice->surfaceFactory());
		if (!m_pFrame)
			return false;

        m_pGrid = pAppVisitor->loadSurface("resources/grid_31x31.png", pDevice->surfaceFactory());
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

    bool setGridInterpolate(GfxDevice * pDevice, const RectI& canvas)
    {
        m_pGrid->setScaleMode(ScaleMode::Interpolate);
        pDevice->setBlitSource(m_pGrid);
        return true;
    }
    
    bool setGridNearest(GfxDevice * pDevice, const RectI& canvas)
    {
        m_pGrid->setScaleMode(ScaleMode::Nearest);
        pDevice->setBlitSource(m_pGrid);
        return true;
    }

	bool	blitPalette(GfxDevice * pDevice, const RectI& canvas)
	{
		pDevice->stretchBlit({ 64,0,190,64 }, RectI( 1,1,3,3 ) );
		pDevice->stretchBlit({ 0,64,64,190 }, RectI( 5,1,3,3 ) );
		pDevice->stretchBlit({ 1,1,60,60 }, RectI(1,5,1,1) );

        pDevice->stretchBlit({ 80,80,30,3 }, RectI(1, 1, 3, 3));

        for( int i = 0 ; i < 10 ; i++ )
            pDevice->stretchBlit({ 80,100+i*5,i,3 }, RectI(1, 1, 3, 3));

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

    bool    blitGrid(GfxDevice * pDevice, const RectI& canvas)
    {
        pDevice->stretchBlit({ 0,0, canvas.w, 31 });

        int ofsY = 31;
        for( int y = 40; y > 0 ; y-- )
        {
            for( int i = 0; i < canvas.w ; i+=y )
            {
                if( m_pGrid->scaleMode() == ScaleMode::Interpolate )
                    pDevice->stretchBlit( {i,ofsY,y,10}, RectF(i*30.f/(canvas.w-1), 0, y*30.f/(canvas.w-1), 10) );
                else
                    pDevice->stretchBlit( {i,ofsY,y,10}, RectF(i*31.f/(canvas.w-0.99f), 0, y*31.f/(canvas.w-0.99f), 10) );

            }
            ofsY += 10;
        }

        return true;
    }


private:
	Surface_p	m_pPalette;
	Surface_p	m_pFrame;
    Surface_p   m_pGrid;
};
