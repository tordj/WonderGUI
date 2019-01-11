#include <testsuites/testsuite.h>

class FillTests : public TestSuite
{
public:
	FillTests()
	{
		name = "FillTests";

		addTest("StraightFill", &FillTests::straightFill );
		addTest("BlendFill", &FillTests::blendFill );
		addTest("SubpixelFill", &FillTests::subPixelFill);
	}


	bool straightFill(GfxDevice * pDevice, const Rect& canvas)
	{
		pDevice->setBlendMode(BlendMode::Replace);
		pDevice->fill(canvas, Color::RosyBrown);
		pDevice->setBlendMode(BlendMode::Blend);

		return true;
	}

	bool blendFill(GfxDevice * pDevice, const Rect& canvas)
	{
		pDevice->setTintColor({ 255,255,255,128 });

		pDevice->fill(canvas, Color::RosyBrown);

		pDevice->setTintColor({ 255,255,255,255 });

		return true;
	}

	bool subPixelFill(GfxDevice * pDevice, const Rect& canvas)
	{
		for (int i = 0; i < 5; i++)
		{
			RectF rect = { 10.f + i * 25.f, 10.f + i * 0.25f, 20.f, 10.f };

			for (int j = 0; j < 5; j++)
			{
				pDevice->fill(rect, Color::White);
				rect.y += 12.f;
				rect.x += 0.25f;
			}
		}

		return true;
	}



};
