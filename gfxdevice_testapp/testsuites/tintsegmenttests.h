#include "testsuites/testsuite.h"

class TintSegmentTests : public TestSuite
{
public:
	TintSegmentTests()
	{
		name = "TintSegmentTests";

		addTest("SegmentsBaseTintX", &TintSegmentTests::setBaseGradientX, &TintSegmentTests::tintedSegments, &TintSegmentTests::resetTint);
		addTest("SegmentsBaseTintY", &TintSegmentTests::setBaseGradientY, &TintSegmentTests::tintedSegments, &TintSegmentTests::resetTint);
		addTest("SegmentsBaseTintXY", &TintSegmentTests::setBaseGradientXY, &TintSegmentTests::tintedSegments, &TintSegmentTests::resetTint);

		addTest("SegmentsNoTint", &TintSegmentTests::segmentsNoTint);
		addTest("SegmentsTintX", &TintSegmentTests::segmentsTintX);
		addTest("SegmentsTintY", &TintSegmentTests::segmentsTintY);
		addTest("SegmentsTintXY", &TintSegmentTests::segmentsTintXY);
		addTest("SegmentsTintYBaseTintX", &TintSegmentTests::setBaseGradientX, &TintSegmentTests::segmentsTintY, &TintSegmentTests::resetTint);

		addTest("SegmentsTintXRot90", &TintSegmentTests::rot90, &TintSegmentTests::segmentsTintX, &TintSegmentTests::resetFlip);
		addTest("SegmentsTintYRot90", &TintSegmentTests::rot90, &TintSegmentTests::segmentsTintY, &TintSegmentTests::resetFlip);
		addTest("SegmentsTintXYRot90", &TintSegmentTests::rot90, &TintSegmentTests::segmentsTintXY, &TintSegmentTests::resetFlip);

		addTest("SegmentsTintXRot180", &TintSegmentTests::rot180, &TintSegmentTests::segmentsTintX, &TintSegmentTests::resetFlip);
		addTest("SegmentsTintYRot180", &TintSegmentTests::rot180, &TintSegmentTests::segmentsTintY, &TintSegmentTests::resetFlip);
		addTest("SegmentsTintXYRot180", &TintSegmentTests::rot180, &TintSegmentTests::segmentsTintXY, &TintSegmentTests::resetFlip);

		addTest("SegmentsTintXRot270", &TintSegmentTests::rot270, &TintSegmentTests::segmentsTintX, &TintSegmentTests::resetFlip);
		addTest("SegmentsTintYRot270", &TintSegmentTests::rot270, &TintSegmentTests::segmentsTintY, &TintSegmentTests::resetFlip);
		addTest("SegmentsTintXYRot270", &TintSegmentTests::rot270, &TintSegmentTests::segmentsTintXY, &TintSegmentTests::resetFlip);

		addTest("SegmentsTintXFlipX", &TintSegmentTests::flipX, &TintSegmentTests::segmentsTintX, &TintSegmentTests::resetFlip);
		addTest("SegmentsTintYFlipX", &TintSegmentTests::flipX, &TintSegmentTests::segmentsTintY, &TintSegmentTests::resetFlip);
		addTest("SegmentsTintXYFlipX", &TintSegmentTests::flipX, &TintSegmentTests::segmentsTintXY, &TintSegmentTests::resetFlip);

		addTest("SegmentsTintXFlipY", &TintSegmentTests::flipY, &TintSegmentTests::segmentsTintX, &TintSegmentTests::resetFlip);
		addTest("SegmentsTintYFlipY", &TintSegmentTests::flipY, &TintSegmentTests::segmentsTintY, &TintSegmentTests::resetFlip);
		addTest("SegmentsTintXYFlipY", &TintSegmentTests::flipY, &TintSegmentTests::segmentsTintXY, &TintSegmentTests::resetFlip);

        addTest("SegmentsTintYBaseTintXY", &TintSegmentTests::setBaseGradientXY, &TintSegmentTests::segmentsTintY, &TintSegmentTests::resetTint);
        addTest("SegmentsTintYBaseTintXYRot90", &TintSegmentTests::rot90SetBaseGradientXY, &TintSegmentTests::segmentsTintY, &TintSegmentTests::resetTint);

	}


	bool init(GfxDevice * pDevice, const RectI& canvas, AppVisitor * pAppVisitor)
	{
		// Generate rainbow segments

		float val = 0;
		int ofs = -canvas.h / 4 * 256;

		for (int i = 0; i <= canvas.w; i++)
		{
			if (i % (canvas.w / 2) > canvas.w / 4)
				val -= 19.1f;
			else
				val += 19.1f;

			m_rainbowEdges[i][0] = ofs + int(val * 32);
			m_rainbowEdges[i][1] = ofs + int(val * 64);
			m_rainbowEdges[i][2] = ofs + int(val * 96);
			m_rainbowEdges[i][3] = ofs + int(val * 128);
		}


		for (int i = 0; i <= canvas.w; i++)
		{
			if (i % (canvas.w / 2) > canvas.w / 4)
				val -= 10.5f;
			else
				val += 12.5f;

			m_rainbowEdges2[i][0] = canvas.h * 256 - int(val * 64);
			m_rainbowEdges2[i][1] = canvas.h * 256 - int(val * 64) + 2047*3;
		}


		return true;
	}

	bool	resetTint(GfxDevice * pDevice, const RectI& canvas)
	{
        m_flip = GfxFlip::Normal;
		pDevice->setTintColor(Color::White);
		pDevice->clearTintGradient();
		return true;
	}
    
	bool	setBaseGradientX(GfxDevice * pDevice, const RectI& canvas)
	{
//		pDevice->setTintColor({ 128,255,255,255 });

//		pDevice->setTintGradient(canvas, { 0,0,0,255 }, { 255,255,255,255 }, { 255,255,255,255 }, { 0,0,0,255 } );
		pDevice->setTintGradient(canvas, { 255,255,255,255 }, { 0,0,0,255 }, { 0,0,0,255 }, { 255,255,255,255 });
		return true;
	}

	bool	setBaseGradientY(GfxDevice * pDevice, const RectI& canvas)
	{
		//		pDevice->setTintColor({ 128,255,255,255 });

		pDevice->setTintGradient(canvas, { 255,255,255,255 }, { 255,255,255,255 }, { 0,0,0,255 }, { 0,0,0,255 });
		return true;
	}

	bool	setBaseGradientXY(GfxDevice * pDevice, const RectI& canvas)
	{
		//		pDevice->setTintColor({ 128,255,255,255 });

		pDevice->setTintGradient(canvas, { 255,255,255,255 }, { 255,0,0,255 }, { 0,0,0,255 }, { 0,255,0,255 });
		return true;
	}

	bool	rot90(GfxDevice* pDevice, const RectI& canvas)
	{
		m_flip = GfxFlip::Rot90;
		return true;
	}

    bool    rot90SetBaseGradientXY(GfxDevice * pDevice, const RectI& canvas)
    {
        //        pDevice->setTintColor({ 128,255,255,255 });

        m_flip = GfxFlip::Rot90;
        pDevice->setTintGradient(canvas, { 255,255,255,255 }, { 255,0,0,255 }, { 0,0,0,255 }, { 0,255,0,255 });
        return true;
    }

    
	bool	rot180(GfxDevice* pDevice, const RectI& canvas)
	{
		m_flip = GfxFlip::Rot180;
		return true;
	}

	bool	rot270(GfxDevice* pDevice, const RectI& canvas)
	{
		m_flip = GfxFlip::Rot270;
		return true;
	}


	bool	flipX(GfxDevice* pDevice, const RectI& canvas)
	{
		m_flip = GfxFlip::FlipX;
		return true;
	}

	bool	flipY(GfxDevice* pDevice, const RectI& canvas)
	{
		m_flip = GfxFlip::FlipY;
		return true;
	}


	bool	resetFlip(GfxDevice* pDevice, const RectI& canvas)
	{
		m_flip = GfxFlip::Normal;
		return true;
	}


	bool tintedSegments(GfxDevice * pDevice, const RectI& canvas)
	{
		pDevice->flipDrawSegments(canvas, 4, m_rainbowColors, m_nRainbowEdges, (int*)m_rainbowEdges, 7, m_flip);
		return true;
	}

	bool segmentsNoTint(GfxDevice * pDevice, const RectI& canvas)
	{
		Color colors[] = { Color::Red, Color::Orange, Color::Green };

		pDevice->flipDrawSegments(canvas, 3, colors, m_nRainbowEdges, (int*)m_rainbowEdges2, 7, m_flip);
		return true;
	}

	bool segmentsTintX(GfxDevice * pDevice, const RectI& canvas)
	{
		Color colors[] = { Color::White, Color::Red, Color::Orange, Color::Orange, Color::Black, Color::Green };

		pDevice->flipDrawSegments(canvas, 3, colors, m_nRainbowEdges, (int*)m_rainbowEdges2, 7, m_flip, TintMode::GradientX);
		return true;
	}


	bool segmentsTintY(GfxDevice * pDevice, const RectI& canvas)
	{
		Color colors[] = { {0,0,0,255}, {255,0,0,255}, Color::Orange, Color::Orange, Color::Blue, Color::Green };

		pDevice->flipDrawSegments(canvas, 3, colors, m_nRainbowEdges, (int*)m_rainbowEdges2, 7, m_flip, TintMode::GradientY);
		return true;
	}

	bool segmentsTintXY(GfxDevice * pDevice, const RectI& canvas)
	{
		Color colors[] = { Color::DeepPink, Color::HotPink, Color::Black, Color::White, Color::Orange, Color::Orange, Color::Orange, Color::Orange, Color::LightBlue, Color::Blue, Color::Green, Color::LightGreen };

		pDevice->flipDrawSegments(canvas, 3, colors, m_nRainbowEdges, (int*)m_rainbowEdges2, 7, m_flip, TintMode::GradientXY );
		return true;
	}


private:

	GfxFlip	m_flip = GfxFlip::Normal;

	Color	m_rainbowColors[8] = { Color::Red, Color::Orange, Color::Yellow, Color::Green, Color::Blue, Color::Indigo, Color::Violet, { 255,255,255,128 } };

	int		m_nRainbowEdges = 512 + 1;
	int		m_rainbowEdges[512 + 1][7];

	int		m_rainbowEdges2[512 + 1][7];


};

