#ifndef TESTFRAMEWORK_DOT_H
#define TESTFRAMEWORK_DOT_H



#include <wg_chain.h>
#include <stdio.h>

//____ Macros __________________________________________________________________

#define DECL_TEST( _class_, _method_ )										\
	bool _method_(void);													\
	static bool _method_##cb(void * p) {return ((_class_*)p)->_method_();}

#define ADD_TEST( _method_ )												\
	m_tests.push_back( new Test( #_method_, _method_ ## cb ) );

#define TEST_ASSERT( _expression_ )											\
	if( !(_expression_) )													\
	{																		\
		sprintf( m_errorMsg, "Failed check '%s' at line %d in file %s\n", 	\
				 #_expression_	, __LINE__, __FILE__ );						\
		return false;														\
	}




//____ Test ____________________________________________________________________


class Test : public WgLink
{
public:
	Test( char * _pName, bool(*_pFunc)(void*) ) { pName = _pName; pFunc = _pFunc; }

	LINK_METHODS( Test );

	const char *  	pName;
	bool(*pFunc)(void*);
};


//____ TestCollection __________________________________________________________

class TestCollection : public WgLink
{
public:
	TestCollection() { m_errorMsg[0] = 0; };
	virtual ~TestCollection() {};

	bool Run();


	bool RunTest( Test * pTest );

	inline Test * GetFirstTest() {return m_tests.getFirst();}
	Test * GetNextTest( Test * p ) {return p->getNext();}

	virtual char * GetName() const = 0;

	LINK_METHODS( TestCollection );

protected:

	char 			m_errorMsg[512];

	WgChain<Test>	m_tests;
};

//____ TestFramework ___________________________________________________________

class TestFramework
{
public:
	TestFramework();
	~TestFramework();

	bool RunAll();
	bool RunCollection( const char * pName );
	bool RunTest( const char * pCollectionName, const char * pTestName );


private:
	inline void AddTestClass( TestCollection * pCollection ) { m_testCollections.push_back(pCollection); };

	WgChain<TestCollection> m_testCollections;

};

#endif // TESTFRAMEWORK_DOT_H
