#include "../testframework.h"

class LineRenderTest : public TestCollection
{
public:
	LineRenderTest();
	~LineRenderTest();

	std::string name() const override { return "Line Render Test"; };

private:

	bool capsCodecTest(std::ostream& output);
	bool capsCommandTest(std::ostream& output);
};
