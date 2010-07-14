


#include "testframework.h"
#include <stdio.h>
#include <string.h>

#include "text_test.h"
#include "valueformat_test.h"
//#include "textparser_test.h"

//____ Constructor _____________________________________________________________

TestFramework::TestFramework()
{
	AddTestClass( new ValueFormatTest() );
//	AddTestClass( new TextTest() );
//	AddTestClass( new TextParserTest() );
}

//____ Destructor ______________________________________________________________

TestFramework::~TestFramework()
{
	// Nothing to do. TestCollections are freed by Chain automatically.
}

//____ RunAll() ________________________________________________________________

bool TestFramework::RunAll()
{
	printf( "Running all tests in the framework:\n" );

	TestCollection * p = m_testCollections.getFirst();

	bool bOk = true;

	while( p && bOk )
	{
		bOk = p->Run();
		p = p->getNext();
	}

	if( bOk )
	{
		printf( "\nAll tests ran successfully!\n\n" );
	}


	return bOk;
}

//____ RunCollection() _________________________________________________________

bool TestFramework::RunCollection( const char * pName )
{

	TestCollection * p = m_testCollections.getFirst();

	while( p )
	{
		if( strcmp( p->GetName(), pName ) == 0 )
		{
			printf( "Running all tests of collection '%s':\n", pName );
			return p->Run();
		}
		p = p->getNext();
	}

	printf( "ERROR: Couldn't find test collection '%s'!\n", pName );

	return false;
}


//____ RunTest() _______________________________________________________________

bool TestFramework::RunTest( const char * pCollectionName, const char * pTestName )
{
	TestCollection * p = m_testCollections.getFirst();

	while( p )
	{
		if( strcmp( p->GetName(), pCollectionName ) == 0 )
			break;

		p = p->getNext();
	}

	if( !p )
	{
		printf( "ERROR: Couldn't find test collection '%s'!\n", pCollectionName );
		return false;
	}


	Test * t = p->GetFirstTest();

	while( t )
	{
		if( strcmp( t->pName, pTestName ) == 0 )
		{
			printf( "Running test '%s' of collection '%s':\n", pTestName, pCollectionName );
			return p->RunTest(t);

		}
		t = p->GetNextTest( t );
	}

	printf( "ERROR: Couldn't find test '%s' of collection '%s'!\n", pTestName, pCollectionName );
	return false;
}


//____ TestCollection::Run() ___________________________________________________

bool TestCollection::Run()
{
	printf( "  Collection: %s\n", GetName() );

	Test * p = m_tests.getFirst();

	bool bOk = true;

	while( p && bOk )
	{
		bOk = RunTest(p);
		p = p->getNext();
	}

	return bOk;
}


//____ TestCollection::RunTest() _______________________________________________

bool TestCollection::RunTest( Test * p )
{
	printf( "    Running %s...", p->pName );

	if( p->pFunc( this ) )
	{
		printf( "  OK\n" );
		return true;
	}
	else
	{
		printf( "  FAILED!\n\n" );
		printf( "ERROR: %s\n", m_errorMsg );
		return false;
	}
}

