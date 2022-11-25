#include "../testframework.h"



class GfxStreamTest : public TestCollection
{
public:
	GfxStreamTest();
	~GfxStreamTest();

	std::string name() const { return "GfxStream Test"; };

	bool init(std::ostream& output) override;

private:

	bool frameOptimizationTest(std::ostream& output);
};
