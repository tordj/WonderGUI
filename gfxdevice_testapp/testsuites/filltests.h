#include <testsuites/testsuite.h>

class FillTests : public TestSuite
{
public:
	FillTests()
	{
		name = "FillTests";

		addTest("StraightFill", &FillTests::straightFill );
		addTest("BlendFill", &FillTests::blendFill );
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


};
