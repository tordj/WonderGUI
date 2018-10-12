#include <testsuites/testsuite.h>

class PatchBlitTests : public TestSuite
{
public:
	PatchBlitTests()
	{
		name = "PatchBlitTests";

		addTest("Blit", &PatchBlitTests::setSplash, &PatchBlitTests::blit, &PatchBlitTests::dummy);
		addTest("BlitPatches", &PatchBlitTests::setSplash, &PatchBlitTests::blitPatches, &PatchBlitTests::dummy );

		addTest("FlipBlit", &PatchBlitTests::setSplash, &PatchBlitTests::flipBlit, &PatchBlitTests::dummy);
		addTest("FlipBlitPatches", &PatchBlitTests::setSplash, &PatchBlitTests::flipBlitPatches, &PatchBlitTests::dummy );

		addTest("StretchBlit", &PatchBlitTests::setSplash, &PatchBlitTests::stretchBlit, &PatchBlitTests::dummy);
		addTest("StretchBlitPatches", &PatchBlitTests::setSplash, &PatchBlitTests::stretchBlitPatches, &PatchBlitTests::dummy );

		addTest("RotScaleBlit", &PatchBlitTests::setClockFace, &PatchBlitTests::rotScaleBlit, &PatchBlitTests::dummy);
		addTest("RotScaleBlitPatches", &PatchBlitTests::setClockFace, &PatchBlitTests::rotScaleBlitPatches, &PatchBlitTests::dummy);
	}

	bool init(GfxDevice * pDevice, const Rect& canvas)
	{
		m_pClockFace = FileUtil::loadSurface("../resources/clockface_2500.png", pDevice->surfaceFactory());
		if (!m_pClockFace)
			return false;
		m_pClockFace->setScaleMode(ScaleMode::Interpolate);

		m_pSplash = FileUtil::loadSurface("../resources/splash_opaque.png", pDevice->surfaceFactory());
		if (!m_pSplash)
			return false;

		genPatches(canvas, m_patches);
		genPatches({ canvas.pos(), m_pSplash->size() }, m_miniPatches);
		return true;
	}
	 
	bool exit(GfxDevice * pDevice, const Rect& canvas)
	{
		m_pClockFace = nullptr;
		m_pSplash = nullptr;
		return true;
	}

	void genPatches(const Rect& canvas, Rect * pPatches)
	{
		pPatches[0] = { canvas.x,canvas.y, canvas.w / 4, canvas.h / 4 };
		pPatches[1] = { canvas.x + canvas.w / 2,canvas.y, canvas.w / 2, canvas.h / 4 };
		pPatches[2] = { canvas.x + canvas.w / 4, canvas.y + canvas.h / 4, canvas.w / 2, canvas.h / 2 };
		pPatches[3] = { canvas.x,canvas.y + canvas.h * 3 / 4, canvas.w / 4, canvas.h / 4 };
		pPatches[4] = { canvas.x + canvas.w * 3 / 4,canvas.y + canvas.h * 3 / 4, canvas.w, canvas.h };
	}

	bool dummy(GfxDevice * pDevice, const Rect& canvas)
	{
		return true;
	}

	bool setClockFace(GfxDevice * pDevice, const Rect& canvas)
	{
		pDevice->setBlitSource(m_pClockFace);
		return true;
	}

	bool setSplash(GfxDevice * pDevice, const Rect& canvas)
	{
		pDevice->setBlitSource(m_pSplash);
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

	bool	blitPatches(GfxDevice * pDevice, const Rect& canvas)
	{
		pDevice->blitPatches(canvas, 5,m_miniPatches);
		return true;
	}

	bool	flipBlitPatches(GfxDevice * pDevice, const Rect& canvas)
	{
		pDevice->flipBlitPatches(canvas, GfxFlip::Rot270,5,m_miniPatches);
		return true;
	}



	bool	rotScaleBlit(GfxDevice * pDevice, const Rect& canvas)
	{
		CoordF center = { m_pClockFace->size().w / 2.f, m_pClockFace->size().h / 2.f };
		pDevice->rotScaleBlit(canvas, center, rot, scale);
		return true;
	}

	bool	rotScaleBlitPatches(GfxDevice * pDevice, const Rect& canvas)
	{
		CoordF center = { m_pClockFace->size().w / 2.f, m_pClockFace->size().h / 2.f };

		pDevice->rotScaleBlitPatches(canvas, center, rot, scale, 5, m_patches);
		return true;
	}

	bool	stretchBlit(GfxDevice * pDevice, const Rect& canvas)
	{
		pDevice->stretchBlit(canvas, Rect( 0,0, m_pSplash->size() ) );
		return true;
	}

	bool	stretchBlitPatches(GfxDevice * pDevice, const Rect& canvas)
	{
		pDevice->stretchBlitPatches(canvas, Rect(0,0, m_pSplash->size() ), 5, m_patches);
		return true;
	}


private:
	Surface_p	m_pClockFace;
	Surface_p	m_pSplash;

	Rect		m_patches[5];
	Rect		m_miniPatches[5];

	float rot = 35;
	float scale = 1.3f;
};