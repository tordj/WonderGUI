#include "testsuite.h"

class FillTests : public TestSuite
{
public:
	FillTests()
	{
		name = "FillTests";

		addTest("StraightFill", &FillTests::straightFill );
		addTest("BlendFill", &FillTests::blendFill );
		addTest("TintFill", &FillTests::tintFill);
		addTest("GradientXFill", &FillTests::gradientXFill);
		addTest("GradientYFill", &FillTests::gradientYFill);
		addTest("GradientXYFill", &FillTests::gradientXYFill);
		addTest("NonGradientFill", &FillTests::nonGradientFill);
		addTest("SubpixelFill", &FillTests::subPixelFill);
		addTest("SubpixelGradientXYFill", &FillTests::subPixelGradientXYFill);
	}




	bool straightFill(GfxDevice * pDevice, const RectI& canvas)
	{
		pDevice->setBlendMode(BlendMode::Replace);
		pDevice->fill(canvas, Color::RosyBrown);
		pDevice->setBlendMode(BlendMode::Blend);

		return true;
	}

	bool blendFill(GfxDevice * pDevice, const RectI& canvas)
	{
		pDevice->setTintColor( Color(255,255,255,128) );

		pDevice->fill(canvas, Color::RosyBrown);

		pDevice->setTintColor(Color::White);

		return true;
	}

	bool tintFill(GfxDevice * pDevice, const RectI& canvas)
	{
		pDevice->setTintColor(Color(255,0,0,255));
		pDevice->fill( RectI(0,0,32,32) + canvas.pos(), Color::White);

		pDevice->setTintColor(Color(0,255,0,255));
		pDevice->fill(RectI(40, 0, 32, 32) + canvas.pos(), Color::White);

		pDevice->setTintColor(Color(0,0,255,255));
		pDevice->fill(RectI(80, 0, 32, 32) + canvas.pos(), Color::White);

		pDevice->setTintColor(Color::White);

		return true;
	}

	bool nonGradientFill(GfxDevice * pDevice, const RectI& canvas)
	{
		pDevice->fill(RectI(0, 0, 64, 64) + canvas.pos(), Color::White);
		pDevice->fill(RectI(64, 0, 256, 80) + canvas.pos(), Color::White);
		pDevice->fill(RectI(canvas.w - 256, canvas.h - 256, 256, 256) + canvas.pos(), Color::White);
		pDevice->fill(RectI(0, canvas.h - 350, 250, 350) + canvas.pos(), Color::White);
		pDevice->fill(RectI(canvas.w - 100, 0, 100, 240) + canvas.pos(), Color::White);

		return true;
	}

	bool gradientXFill(GfxDevice * pDevice, const RectI& canvas)
	{
		pDevice->setTintGradient(canvas,  Gradient(Placement::West, Color(255,0,0,255), Color(0,0,255,255)));
		pDevice->fill(RectI(0, 0, 64, 64) + canvas.pos(), Color::White);
		pDevice->fill(RectI(64, 0, 256, 80) + canvas.pos(), Color::White);
		pDevice->fill(RectI(canvas.w-256, canvas.h-256, 256, 256) + canvas.pos(), Color::White);
		pDevice->fill(RectI(0, canvas.h - 350, 250, 350) + canvas.pos(), Color::White);
		pDevice->fill(RectI(canvas.w-100, 0, 100, 240) + canvas.pos(), Color::White);

		pDevice->clearTintGradient();

		return true;
	}

	bool gradientYFill(GfxDevice * pDevice, const RectI& canvas)
	{
		pDevice->setTintGradient(canvas, Gradient(Placement::North, Color(255,0,0,255), Color(0,0,255,255)));
		pDevice->fill(RectI(0, 0, 64, 64) + canvas.pos(), Color::White);
		pDevice->fill(RectI(64, 0, 256, 80) + canvas.pos(), Color::White);
		pDevice->fill(RectI(canvas.w - 256, canvas.h - 256, 256, 256) + canvas.pos(), Color::White);
		pDevice->fill(RectI(0, canvas.h - 350, 250, 350) + canvas.pos(), Color::White);
		pDevice->fill(RectI(canvas.w - 100, 0, 100, 240) + canvas.pos(), Color::White);

		pDevice->clearTintGradient();

		return true;
	}

	bool gradientXYFill(GfxDevice * pDevice, const RectI& canvas)
	{
		pDevice->setTintGradient(canvas, Gradient(Placement::NorthWest, Color(255,0,0,255), Color(0,0,255,255)));
		pDevice->fill(RectI(0, 0, 64, 64) + canvas.pos(), Color::White);
		pDevice->fill(RectI(64, 0, 256, 80) + canvas.pos(), Color::White);
		pDevice->fill(RectI(canvas.w - 256, canvas.h - 256, 256, 256) + canvas.pos(), Color::White);
		pDevice->fill(RectI(0, canvas.h - 350, 250, 350) + canvas.pos(), Color::White);
		pDevice->fill(RectI(canvas.w - 100, 0, 100, 240) + canvas.pos(), Color::White);

		pDevice->clearTintGradient();

		return true;
	}



	bool subPixelFill(GfxDevice * pDevice, const RectI& canvas)
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

	bool subPixelGradientXYFill(GfxDevice* pDevice, const RectI& canvas)
	{
		pDevice->setTintGradient(canvas, Gradient(Placement::NorthWest, Color(255,0,0,255), Color(0,0,255,255)));

		for (int i = 0; i < 10; i++)
		{
			RectF rect = { 10.f + i * 25.f, 10.f + i * 0.25f, 20.f, 10.f };

			for (int j = 0; j < 10; j++)
			{
				pDevice->fill(rect, Color::White);
				rect.y += 12.f;
				rect.x += 0.25f;
			}
		}

		pDevice->clearTintGradient();

		return true;
	}




};
