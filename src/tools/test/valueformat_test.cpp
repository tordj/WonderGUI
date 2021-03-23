#include "valueformat_test.h"

#include <wg_valueformat.h>
#include <wg_charbuffer.h>

ValueFormatTest::ValueFormatTest()
{

	ADD_TEST( InitFromString );
}

ValueFormatTest::~ValueFormatTest()
{
}

bool ValueFormatTest::InitFromString()
{
	WgValueFormat	format1("1000");

	TEST_ASSERT( format1.decimals == 0 );
	TEST_ASSERT( format1.grouping == 0 );
	TEST_ASSERT( format1.prefix[0] == 0 );
	TEST_ASSERT( format1.suffix[0] == 0 );


	WgValueFormat	format2("1;000");

	TEST_ASSERT( format2.decimals == 0 );
	TEST_ASSERT( format2.grouping == 3 );
	TEST_ASSERT( format2.separator == ';' );
	TEST_ASSERT( format2.prefix[0] == 0 );
	TEST_ASSERT( format2.suffix[0] == 0 );

	WgValueFormat	format3("1;000:0");

	TEST_ASSERT( format3.decimals == 1 );
	TEST_ASSERT( format3.grouping == 3 );
	TEST_ASSERT( format3.separator == ';' );
	TEST_ASSERT( format3.period == ':' );
	TEST_ASSERT( format3.prefix[0] == 0 );
	TEST_ASSERT( format3.suffix[0] == 0 );
	TEST_ASSERT( format3.bForceDecimals == true );

	WgValueFormat	format4("1;00:000");

	TEST_ASSERT( format4.decimals == 3 );
	TEST_ASSERT( format4.grouping == 2 );
	TEST_ASSERT( format4.separator == ';' );
	TEST_ASSERT( format4.period == ':' );
	TEST_ASSERT( format4.prefix[0] == 0 );
	TEST_ASSERT( format4.suffix[0] == 0 );
	TEST_ASSERT( format4.bForceDecimals == true );

	WgValueFormat	format5("100:000");

	TEST_ASSERT( format5.decimals == 3 );
	TEST_ASSERT( format5.grouping == 0 );
	TEST_ASSERT( format5.period == ':' );
	TEST_ASSERT( format5.prefix[0] == 0 );
	TEST_ASSERT( format5.suffix[0] == 0 );
	TEST_ASSERT( format5.bForceDecimals == true );


	std::string	a = std::string("$\xc2\xa3\xe2\x82\xac") + std::string("1;000000:0000 USD");		// string is '$£€1;000000:0000 USD'
	WgValueFormat	format6( a );

	TEST_ASSERT( format6.decimals == 4 );
	TEST_ASSERT( format6.grouping == 6 );
	TEST_ASSERT( format6.separator == ';' );
	TEST_ASSERT( format6.prefix[0] == '$' );
	TEST_ASSERT( format6.prefix[1] == 0xa3 );	// pound sign
	TEST_ASSERT( format6.prefix[2] == 0x20ac );	// euro sign
	TEST_ASSERT( format6.prefix[3] == 0 );
	TEST_ASSERT( format6.suffix[0] == ' ' );
	TEST_ASSERT( format6.suffix[1] == 'U' );
	TEST_ASSERT( format6.suffix[2] == 'S' );
	TEST_ASSERT( format6.suffix[3] == 'D' );
	TEST_ASSERT( format6.bForceDecimals == true );

	return true;
}