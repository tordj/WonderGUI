#include "../testframework.h"


class FrostTest : public TestCollection
{
public:
	FrostTest();
	~FrostTest();

	std::string name() const override { return "Frost Test"; };

private:

	bool frostBlursBackdrop(std::ostream& output);
	bool frostIsCached(std::ostream& output);
	bool frostMask(std::ostream& output);
	bool paneOutsideBackdrop(std::ostream& output);
	bool slotChanges(std::ostream& output);
};
