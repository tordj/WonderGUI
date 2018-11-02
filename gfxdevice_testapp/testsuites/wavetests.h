#include <testsuites/testsuite.h>

class WaveTests : public TestSuite
{
public:
	WaveTests()
	{
		name = "WaveTests";

		addTest("SimpleWave", &WaveTests::simpleWave );
		addTest("SimpleWavePatches", &WaveTests::simpleWavePatches);
		addTest("SimpleWaveRot90Patches", &WaveTests::simpleWaveRot90Patches);
		addTest("SimpleWaveRot180Patches", &WaveTests::simpleWaveRot180Patches);
		addTest("SimpleWaveRot270Patches", &WaveTests::simpleWaveRot270Patches);
		addTest("SimpleWaveFlipXPatches", &WaveTests::simpleWaveFlipXPatches);
		addTest("SimpleWaveFlipYPatches", &WaveTests::simpleWaveFlipYPatches);
		addTest("SimpleWaveClipPatches", &WaveTests::simpleWaveClipPatches);
		addTest("SimpleWaveClipRot90Patches", &WaveTests::simpleWaveClipRot90Patches);
		addTest("SimpleWaveClipRot180Patches", &WaveTests::simpleWaveClipRot180Patches);
		addTest("SimpleWaveClipRot270Patches", &WaveTests::simpleWaveClipRot270Patches);
		addTest("BackfaceWave", &WaveTests::backfaceWave);
		addTest("BackfaceWaveSameColor", &WaveTests::backfaceWaveSameColor);

	}


	bool init(GfxDevice * pDevice, const Rect& canvas)
	{

		// Generate standard patch pattern

		m_patches[0] = { canvas.x,canvas.y, canvas.w / 4, canvas.h / 4 };
		m_patches[1] = { canvas.x + canvas.w / 2,canvas.y, canvas.w / 2, canvas.h / 4 };
		m_patches[2] = { canvas.x + canvas.w / 4, canvas.y + canvas.h / 4, canvas.w / 2, canvas.h / 2 };
		m_patches[3] = { canvas.x,canvas.y + canvas.h * 3 / 4, canvas.w / 4, canvas.h / 4 };
		m_patches[4] = { canvas.x + canvas.w * 3 / 4,canvas.y + canvas.h * 3 / 4, canvas.w, canvas.h };

		// Generate simple wave

		m_simpleWaveTopLine.color = Color::White;
		m_simpleWaveTopLine.hold = canvas.h / 2 * 256;
		m_simpleWaveTopLine.length = 513;
		m_simpleWaveTopLine.pWave = m_simpleWaveTopSamples;
		m_simpleWaveTopLine.thickness = 1.f;

		m_simpleWaveBottomLine.color = Color::HotPink;
		m_simpleWaveBottomLine.hold = canvas.h / 2 * 256;
		m_simpleWaveBottomLine.length = 30;
		m_simpleWaveBottomLine.pWave = m_simpleWaveBottomSamples;
		m_simpleWaveBottomLine.thickness = 5.f;

		for (int i = 0; i < 512+1; i++)
		{
			m_simpleWaveTopSamples[i] = (int)((100+sin(i / 10.0) * 80) * 256);
			m_simpleWaveBottomSamples[i] = (int)((300 + sin(i / 20.0) * 6) * 256);
		}

		// Generate backface wave

		m_backfaceWaveTopLine.color = Color::White;
		m_backfaceWaveTopLine.hold = canvas.h / 2 * 256;
		m_backfaceWaveTopLine.length = 513;
		m_backfaceWaveTopLine.pWave = m_backfaceWaveTopSamples;
		m_backfaceWaveTopLine.thickness = 1.f;

		m_backfaceWaveBottomLine.color = Color::HotPink;
		m_backfaceWaveBottomLine.hold = canvas.h / 2 * 256;
		m_backfaceWaveBottomLine.length = 513;
		m_backfaceWaveBottomLine.pWave = m_backfaceWaveBottomSamples;
		m_backfaceWaveBottomLine.thickness = 5.f;

		for (int i = 0; i < 512 + 1; i++)
		{
			m_backfaceWaveTopSamples[i] = (int)((100 + sin(i / 10.0) * 80) * 256);
			m_backfaceWaveBottomSamples[i] = (int)((100 + sin(i / 20.0) * 6) * 256);
		}


		return true;
	}



	bool simpleWave(GfxDevice * pDevice, const Rect& canvas)
	{
		pDevice->drawWave(canvas, &m_simpleWaveTopLine, &m_simpleWaveBottomLine, Color::Green, Color::Red);
		return true;
	}

	bool simpleWavePatches(GfxDevice * pDevice, const Rect& canvas)
	{
		pDevice->drawWavePatches(canvas, &m_simpleWaveTopLine, &m_simpleWaveBottomLine, Color::Green, Color::Red, 5, m_patches);
		return true;
	}

	bool simpleWaveRot90Patches(GfxDevice * pDevice, const Rect& canvas)
	{
		pDevice->flipDrawWavePatches(canvas, &m_simpleWaveTopLine, &m_simpleWaveBottomLine, Color::Green, Color::Red, GfxFlip::Rot90, 5, m_patches);
		return true;
	}

	bool simpleWaveRot180Patches(GfxDevice * pDevice, const Rect& canvas)
	{
		pDevice->flipDrawWavePatches(canvas, &m_simpleWaveTopLine, &m_simpleWaveBottomLine, Color::Green, Color::Red, GfxFlip::Rot180, 5, m_patches);
		return true;
	}

	bool simpleWaveRot270Patches(GfxDevice * pDevice, const Rect& canvas)
	{
		pDevice->flipDrawWavePatches(canvas, &m_simpleWaveTopLine, &m_simpleWaveBottomLine, Color::Green, Color::Red, GfxFlip::Rot270, 5, m_patches);
		return true;
	}

	bool simpleWaveFlipXPatches(GfxDevice * pDevice, const Rect& canvas)
	{
		pDevice->flipDrawWavePatches(canvas, &m_simpleWaveTopLine, &m_simpleWaveBottomLine, Color::Green, Color::Red, GfxFlip::FlipX, 5, m_patches);
		return true;
	}

	bool simpleWaveFlipYPatches(GfxDevice * pDevice, const Rect& canvas)
	{
		Rect clip = pDevice->clip();
		pDevice->setClip(clip - Border(10, 20, 30, 1));
		pDevice->flipDrawWavePatches(canvas, &m_simpleWaveTopLine, &m_simpleWaveBottomLine, Color::Green, Color::Red, GfxFlip::FlipY, 5, m_patches);
		pDevice->setClip(clip);
		return true;
	}

	bool simpleWaveClipPatches(GfxDevice * pDevice, const Rect& canvas)
	{
		Rect clip = pDevice->clip();
		pDevice->setClip(clip - Border(10, 20, 30, 1));
		pDevice->drawWavePatches(canvas, &m_simpleWaveTopLine, &m_simpleWaveBottomLine, Color::Green, Color::Red, 5, m_patches);
		pDevice->setClip(clip);
		return true;
	}

	bool simpleWaveClipRot90Patches(GfxDevice * pDevice, const Rect& canvas)
	{
		Rect clip = pDevice->clip();
		pDevice->setClip(clip - Border(10, 20, 30, 1));
		pDevice->flipDrawWavePatches(canvas, &m_simpleWaveTopLine, &m_simpleWaveBottomLine, Color::Green, Color::Red, GfxFlip::Rot90, 5, m_patches);
		pDevice->setClip(clip);
		return true;
	}

	bool simpleWaveClipRot180Patches(GfxDevice * pDevice, const Rect& canvas)
	{
		Rect clip = pDevice->clip();
		pDevice->setClip(clip - Border(10, 20, 30, 1));
		pDevice->flipDrawWavePatches(canvas, &m_simpleWaveTopLine, &m_simpleWaveBottomLine, Color::Green, Color::Red, GfxFlip::Rot180, 5, m_patches);
		pDevice->setClip(clip);
		return true;
	}

	bool simpleWaveClipRot270Patches(GfxDevice * pDevice, const Rect& canvas)
	{
		Rect clip = pDevice->clip();
		pDevice->setClip(clip - Border(10, 20, 30, 1));
		pDevice->flipDrawWavePatches(canvas, &m_simpleWaveTopLine, &m_simpleWaveBottomLine, Color::Green, Color::Red, GfxFlip::Rot270, 5, m_patches);
		pDevice->setClip(clip);
		return true;
	}

	bool backfaceWave(GfxDevice * pDevice, const Rect& canvas)
	{
		pDevice->drawWave(canvas, &m_backfaceWaveTopLine, &m_backfaceWaveBottomLine, Color::Green, Color::Red );
		return true;
	}

	bool backfaceWaveSameColor(GfxDevice * pDevice, const Rect& canvas)
	{
		pDevice->drawWave(canvas, &m_backfaceWaveTopLine, &m_backfaceWaveBottomLine, Color::Green, Color::Green);
		return true;
	}


private:

	WaveLine	m_simpleWaveTopLine;
	WaveLine	m_simpleWaveBottomLine;

	int			m_simpleWaveTopSamples[512 + 1];
	int			m_simpleWaveBottomSamples[512 + 1];

	WaveLine	m_backfaceWaveTopLine;
	WaveLine	m_backfaceWaveBottomLine;

	int			m_backfaceWaveTopSamples[512 + 1];
	int			m_backfaceWaveBottomSamples[512 + 1];



	Color	m_rainbowColors[8] = { Color::Red, Color::Orange, Color::Yellow, Color::Green, Color::Blue, Color::Indigo, Color::Violet, { 255,255,255,128 } };

	Rect	m_patches[5];

};
 