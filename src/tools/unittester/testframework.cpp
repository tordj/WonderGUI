


#include "testframework.h"
#include <stdio.h>
#include <string.h>

#include "tests/gfxstream.h"

//____ Constructor _____________________________________________________________

TestFramework::TestFramework( std::ostream& output ) :
	m_log(output)
{

	addCollection( new GfxStreamTest() );
}

//____ Destructor ______________________________________________________________

TestFramework::~TestFramework()
{
	for (auto p : m_testCollections)
		delete p;
}

//____ initAllTests() ________________________________________________________________

int TestFramework::initAllTests()
{
	m_log << "Initializing Tests" << std::endl;

	int nFailed = 0;
	for (auto p : m_testCollections)
	{
		if (!p->init(m_log))
			nFailed++;
	}

	return nFailed;
}

//____ runAllTests() ________________________________________________________________

int TestFramework::runAllTests()
{
	m_log << "Running all tests in the framework:\n";

	int nFailures = 0;
	for (auto p : m_testCollections)
	{
		for (int i = 0; i < p->nbTests(); i++)
		{
			if (!p->getTest(i).run(m_log))
				nFailures++;
		}
	}

	if( nFailures == 0 )
	{
		m_log << "\nAll tests ran successfully!\n\n" << std::flush;
	}
	else
	{
		m_log << "\n" << nFailures << " tests failed!\n\n" << std::flush;
	}

	return nFailures;
}

//____ runCollection() _________________________________________________________

int TestFramework::runCollection( int collIdx )
{
	TestCollection* pCollection = m_testCollections[collIdx];

	m_log << "Running all tests of collection '" << pCollection->name() << "':\n";

	int nFailures = 0;
	for (int i = 0; i < pCollection->nbTests(); i++)
	{
		if (!pCollection->getTest(i).run(m_log))
			nFailures++;
	}

	if (nFailures == 0)
	{
		m_log << "\nAll tests ran successfully!\n\n" << std::flush;
	}
	else
	{
		m_log << "\n" << nFailures << " tests failed!\n\n" << std::flush;
	}

	return nFailures;
}


//____ RunTest() _______________________________________________________________

bool TestFramework::runTest( int collIdx, int testIdx )
{
	TestCollection * p = m_testCollections[collIdx];

	Test& test = p->getTest(testIdx);

	m_log << "Running test '" << test.name() << "' of collection '" << p->name() << "':\n";
	bool bSuccess = test.run(m_log);

	return bSuccess;
}
