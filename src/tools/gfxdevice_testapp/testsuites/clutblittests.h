#include "testsuites/testsuite.h"

class ClutBlitTests : public TestSuite
{
public:
	ClutBlitTests()
	{
		name = "ClutBlitTests";

		addTest("ClutBlit", &ClutBlitTests::setNearestPicture, &ClutBlitTests::blit, &ClutBlitTests::dummy);
		addTest("ClutRotScaleBlit", &ClutBlitTests::setNearestPicture, &ClutBlitTests::rotScaleBlit, &ClutBlitTests::dummy);
		addTest("ClutRotScaleBlitInterpolate", &ClutBlitTests::setInterpolatedPicture, &ClutBlitTests::rotScaleBlit, &ClutBlitTests::dummy);
		addTest("ClutStretchBlit", &ClutBlitTests::setNearestPicture, &ClutBlitTests::stretchBlit, &ClutBlitTests::dummy);
		addTest("ClutStretchBlitInterpolated", &ClutBlitTests::setInterpolatedPicture, &ClutBlitTests::stretchBlit, &ClutBlitTests::dummy);

	}

	bool init(GfxDevice * pDevice, const RectI& canvas, AppVisitor * pAppVisitor)
	{
		m_pLeaves = pAppVisitor->loadSurface("../resources/BLAETTER.GIF", pDevice->surfaceFactory());
		if (!m_pLeaves)
			return false;

		m_pLeavesInterpolated = pAppVisitor->loadSurface("../resources/BLAETTER.GIF", pDevice->surfaceFactory());
		if (!m_pLeavesInterpolated)
			return false;

		m_pLeavesInterpolated->setScaleMode(ScaleMode::Interpolate);
		m_pLeaves->setScaleMode(ScaleMode::Nearest);

		return true;
	}

	bool exit(GfxDevice * pDevice, const RectI& canvas)
	{
		m_pLeaves = nullptr;
		return true;
	}


	bool dummy(GfxDevice * pDevice, const RectI& canvas)
	{
		return true;
	}

	bool setNearestPicture(GfxDevice * pDevice, const RectI& canvas)
	{
		pDevice->setBlitSource(m_pLeaves);
		return true;
	}

	bool setInterpolatedPicture(GfxDevice * pDevice, const RectI& canvas)
	{
		pDevice->setBlitSource(m_pLeavesInterpolated);
		return true;
	}


	bool	blit(GfxDevice * pDevice, const RectI& canvas)
	{
		pDevice->blit(canvas);
		return true;
	}

	bool	flipBlit(GfxDevice * pDevice, const RectI& canvas)
	{
		pDevice->flipBlit(canvas, GfxFlip::Rot270);
		return true;
	}



	bool	rotScaleBlit(GfxDevice * pDevice, const RectI& canvas)
	{
		pDevice->rotScaleBlit(canvas, rot, scale);
		return true;
	}

	bool	stretchBlit(GfxDevice * pDevice, const RectI& canvas)
	{
		pDevice->stretchBlit(canvas, RectI(0, 0, m_pLeaves->size()));
		return true;
	}

private:
	Surface_p	m_pLeaves;
	Surface_p	m_pLeavesInterpolated;

	float rot = 35;
	float scale = 5.0f;
};
