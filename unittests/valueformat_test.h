#include "testframework.h"



class ValueFormatTest : public TestCollection
{
public:
	ValueFormatTest();
	~ValueFormatTest();

	char * GetName() const { return "ValueFormatTest"; };

private:
	DECL_TEST( ValueFormatTest, InitFromString );
};
