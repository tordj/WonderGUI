#include "testsuite.h"

class BlendTests : public TestSuite
{
public:
	BlendTests()
	{
		name = "BlendTests";

		addTest("BlendModeReplace", &BlendTests::setReplace, &BlendTests::draw, &BlendTests::cleanup);
		addTest("BlendModeBlend", &BlendTests::setBlend, &BlendTests::draw, &BlendTests::cleanup);
		addTest("BlendModeAdd", &BlendTests::setAdd, &BlendTests::draw, &BlendTests::cleanup);
		addTest("BlendModeSubtract", &BlendTests::setSubtract, &BlendTests::draw, &BlendTests::cleanup);
		addTest("BlendModeMultiply", &BlendTests::setMultiply, &BlendTests::draw, &BlendTests::cleanup);
		addTest("BlendModeMin", &BlendTests::setMin, &BlendTests::draw, &BlendTests::cleanup);
		addTest("BlendModeMax", &BlendTests::setMax, &BlendTests::draw, &BlendTests::cleanup);
		addTest("BlendModeInvert", &BlendTests::setInvert, &BlendTests::draw, &BlendTests::cleanup);
		addTest("BlendModeIgnore", &BlendTests::setIgnore, &BlendTests::draw, &BlendTests::cleanup);
//		addTest("BlendModeUndefined", &BlendTests::setUndefined, &BlendTests::draw, &BlendTests::cleanup);
	}


	bool init(GfxDevice * pDevice, const RectI& canvas, AppVisitor * pAppVisitor)
	{
		return true;
	}

	bool exit(GfxDevice * pDevice, const RectI& canvas)
	{
		return true;
	}


	bool cleanup(GfxDevice * pDevice, const RectI& canvas)
	{
		pDevice->setBlendMode(BlendMode::Blend);
		return true;
	}

	bool setReplace(GfxDevice * pDevice, const RectI& canvas)
	{
		pDevice->setBlendMode(BlendMode::Replace);
		return true;
	}

	bool setBlend(GfxDevice * pDevice, const RectI& canvas)
	{
		pDevice->setBlendMode(BlendMode::Blend);
		return true;
	}

	bool setAdd(GfxDevice * pDevice, const RectI& canvas)
	{
		pDevice->setBlendMode(BlendMode::Add);
		return true;
	}

	bool setSubtract(GfxDevice * pDevice, const RectI& canvas)
	{
		pDevice->setBlendMode(BlendMode::Subtract);
		return true;
	}

	bool setMultiply(GfxDevice * pDevice, const RectI& canvas)
	{
		pDevice->setBlendMode(BlendMode::Multiply);
		return true;
	}

	bool setMin(GfxDevice * pDevice, const RectI& canvas)
	{
		pDevice->setBlendMode(BlendMode::Min);
		return true;
	}

	bool setMax(GfxDevice * pDevice, const RectI& canvas)
	{
		pDevice->setBlendMode(BlendMode::Max);
		return true;
	}

	bool setInvert(GfxDevice * pDevice, const RectI& canvas)
	{
		pDevice->setBlendMode(BlendMode::Invert);
		return true;
	}

	bool setIgnore(GfxDevice * pDevice, const RectI& canvas)
	{
		pDevice->setBlendMode(BlendMode::Ignore);
		return true;
	}

	bool setUndefined(GfxDevice * pDevice, const RectI& canvas)
	{
		pDevice->setBlendMode(BlendMode::Undefined);
		return true;
	}

	bool draw(GfxDevice * pDevice, const RectI& canvas)
	{
		pDevice->fill( RectI(100, 100, 100, 100), Color8(255, 0, 0, 255));
		pDevice->fill( RectI(150, 100, 100, 100), Color8(0, 255, 0, 255));
		pDevice->fill( RectI(100, 150, 100, 100), Color8(0, 0, 255, 255));
		return true;
	}

};
