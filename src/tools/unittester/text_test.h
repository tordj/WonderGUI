#include "testframework.h"



class TextTest : public TestCollection
{
public:
	TextTest();
	~TextTest();

	char * GetName() const { return "TextTest"; };

private:
	DECL_TEST( TextTest, ManipulatingSimpleTextContent );
	DECL_TEST( TextTest, PropPtrReferenceCounting );
	DECL_TEST( TextTest, PropertiesOnTextObjects );
	DECL_TEST( TextTest, PropertiesOnWgCharArrays );
};
