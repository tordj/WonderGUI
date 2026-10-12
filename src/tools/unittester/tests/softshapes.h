
#include "../testframework.h"

#include <wg_softshapes.h>

class SoftShapesTest : public TestCollection
{
public:
	SoftShapesTest();
	~SoftShapesTest();

	std::string name() const override { return "SoftShapes Test"; };

private:

	bool roundedBoxCoverageTest(std::ostream& output);
	bool roundedBoxSeamTest(std::ostream& output);
	bool roundedBoxSymmetryTest(std::ostream& output);
	bool roundedBoxClipTest(std::ostream& output);
	bool clampCornerRadiiTest(std::ostream& output);
};
