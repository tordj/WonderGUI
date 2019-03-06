#include <testsuites/testsuite.h>

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

	bool init(GfxDevice * pDevice, const Rect& canvas)
	{
		m_pLeaves = FileUtil::loadSurface("../resources/BLAETTER.GIF", pDevice->surfaceFactory());
		if (!m_pLeaves)
			return false;

		m_pLeavesInterpolated = FileUtil::loadSurface("../resources/BLAETTER.GIF", pDevice->surfaceFactory());
		if (!m_pLeavesInterpolated)
			return false;

		m_pLeavesInterpolated->setScaleMode(ScaleMode::Interpolate);

		return true;
	}

	bool exit(GfxDevice * pDevice, const Rect& canvas)
	{
		m_pLeaves = nullptr;
		return true;
	}


	bool dummy(GfxDevice * pDevice, const Rect& canvas)
	{
		return true;
	}

	bool setNearestPicture(GfxDevice * pDevice, const Rect& canvas)
	{
		pDevice->setBlitSource(m_pLeaves);
		return true;
	}

	bool setInterpolatedPicture(GfxDevice * pDevice, const Rect& canvas)
	{
		pDevice->setBlitSource(m_pLeavesInterpolated);
		return true;
	}


	bool	blit(GfxDevice * pDevice, const Rect& canvas)
	{
		pDevice->blit(canvas);
		return true;
	}

	bool	flipBlit(GfxDevice * pDevice, const Rect& canvas)
	{
		pDevice->flipBlit(canvas, GfxFlip::Rot270);
		return true;
	}



	bool	rotScaleBlit(GfxDevice * pDevice, const Rect& canvas)
	{
		CoordF center = { m_pLeaves->size().w / 2.f, m_pLeaves->size().h / 2.f };
		pDevice->rotScaleBlit(canvas, center, rot, scale);
		return true;
	}

	bool	stretchBlit(GfxDevice * pDevice, const Rect& canvas)
	{
		pDevice->stretchBlit(canvas, Rect(0, 0, m_pLeaves->size()));
		return true;
	}

private:
	Surface_p	m_pLeaves;
	Surface_p	m_pLeavesInterpolated;

	float rot = 35;
	float scale = 5.0f;
};