#include <string.h>

#include "gfxstream.h"

#include <wg_string.h>

GfxStreamTest::GfxStreamTest()
{

	ADD_TEST(frameOptimizationTest);

}

GfxStreamTest::~GfxStreamTest()
{
}


bool GfxStreamTest::init(std::ostream& output)
{
	return true;
}


bool GfxStreamTest::frameOptimizationTest(std::ostream& output)
{
	TEST_ASSERT(true == false);

	return true;
}

