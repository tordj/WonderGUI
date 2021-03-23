//#include <conio.h>
#include "testframework.h"


int main( int argc, char * argv[] )
{
	TestFramework	t;

	bool res = t.RunAll();

//	_getch();

	if( res )
		return 0;
	else
		return -1;
}
