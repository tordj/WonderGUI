//#include <conio.h>
#include "testframework.h"


int main( int argc, char * argv[] )
{
	TestFramework	t(std::cout);

	bool res = t.runAllTests();

//	_getch();

	if( res )
		return 0;
	else
		return -1;
}
