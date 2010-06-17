#include <string.h>

#include "text_test.h"

#include <wg_textprop.h>
#include <wg_text.h>

TextTest::TextTest()
{

	ADD_TEST( PropPtrReferenceCounting );
	ADD_TEST( PropertiesOnTextObjects );
	ADD_TEST( PropertiesOnWgCharArrays );
	ADD_TEST( ManipulatingSimpleTextContent );

}

TextTest::~TextTest()
{
}

bool TextTest::ManipulatingSimpleTextContent()
{
	WgText	text1;

	char	buffer[256];

	// Test result from empty contstructor...

	TEST_ASSERT( text1.nbChars() == 0 );
	TEST_ASSERT( text1.nbLines() == 1 );

	buffer[0] = 1;
	TEST_ASSERT( 0 == text1.getTextUTF8(buffer,2) );
	TEST_ASSERT( buffer[0] == 0 );

	// Test clearing an empty WgText...

	text1.clear();

	TEST_ASSERT( text1.nbChars() == 0 );
	TEST_ASSERT( text1.nbLines() == 1 );

	buffer[0] = 1;
	TEST_ASSERT( 0 == text1.getTextUTF8(buffer,2) );
	TEST_ASSERT( buffer[0] == 0 );

	// Set a textstring and make sure everything is ok

	text1.setText( "My little testtext" );

	TEST_ASSERT( text1.nbChars() == 18 );
	TEST_ASSERT( text1.nbLines() == 1 );

	// Test adding some text...

	text1.addText( " is..." );

	TEST_ASSERT( text1.nbChars() == 18+6 );
	TEST_ASSERT( text1.nbLines() == 1 );

	TEST_ASSERT( text1.getTextUTF8(buffer,256) == 18+6 );
	TEST_ASSERT( strcmp( buffer, "My little testtext is..." ) == 0 );

	// ...


	return true;
}


bool TextTest::PropPtrReferenceCounting()
{
	// Check so everything is in order to start the test

	TEST_ASSERT( WgTextPropManager::GetNbProperties() == 1 );
	TEST_ASSERT( WgTextPropManager::GetPropBufferSize() == 1 );

	// Test partly overlapping assignments

	WgTextProp		prop;

	WgTextPropPtr	p1;

	prop.SetUnderlined();
	prop.SetSelected();
	WgTextPropPtr	p2 = prop.Register();

	prop.ClearSelected();
	WgTextPropPtr	p3 = prop.Register();

	prop.SetSelected();
	WgTextPropPtr	p4= prop.Register();

	TEST_ASSERT( WgTextPropManager::GetNbProperties() == 3 );
	TEST_ASSERT( p2.GetHandle() == p4.GetHandle() );


	TEST_ASSERT( WgTextPropManager::GetRefCnt( p1.GetHandle()) == 2 );
	TEST_ASSERT( WgTextPropManager::GetRefCnt( p2.GetHandle()) == 2 );
	TEST_ASSERT( WgTextPropManager::GetRefCnt( p3.GetHandle()) == 1 );

	// Test dummy copy between two identical pointers

	p2 = p4;

	TEST_ASSERT( WgTextPropManager::GetNbProperties() == 3 );
	TEST_ASSERT( WgTextPropManager::GetRefCnt( p2.GetHandle() ) == 2 );

	// Test copy between two non-identical pointers, erasing one attribute...

	p3 = p1;

	TEST_ASSERT( WgTextPropManager::GetNbProperties() == 2 );
	TEST_ASSERT( WgTextPropManager::GetRefCnt( p1.GetHandle() ) == 3 );

	// Test copy constructor...

	WgTextPropPtr p5 = p4;


	TEST_ASSERT( WgTextPropManager::GetNbProperties() == 2 );
	TEST_ASSERT( WgTextPropManager::GetRefCnt( p4.GetHandle() ) == 3 );

	// Remove all references except one, should keep attribute...

	p4 = 0;
	p5 = p4;

	TEST_ASSERT( WgTextPropManager::GetNbProperties() == 2 );
	TEST_ASSERT( WgTextPropManager::GetRefCnt( p2.GetHandle() ) == 1 );

	// Set p5 to itself when we only got one reference... shouldn't do anything...

	p2 = p2;

	TEST_ASSERT( WgTextPropManager::GetNbProperties() == 2 );
	TEST_ASSERT( WgTextPropManager::GetRefCnt( p2.GetHandle() ) == 1 );

	// Remove last reference, should kill attribute...

	p2 = 0;
	TEST_ASSERT( WgTextPropManager::GetNbProperties() == 1 );

	// We should be back to dummy buffer now...

	TEST_ASSERT( WgTextPropManager::GetPropBufferSize() == 1 );

	return true;
}


bool TextTest::PropertiesOnTextObjects()
{
	// Check so everything is in order to start the test

	TEST_ASSERT( WgTextPropManager::GetNbProperties() == 1 );
	TEST_ASSERT( WgTextPropManager::GetPropBufferSize() == 1 );

	// Create a few simple text objects

	WgText	text1( "This is a test text!\n" );
	WgText	text2( "This is another one!\n" );

	// Make sure we only have 2 refs + 1 extra. Empty chars
	// shouldn't have any refs.

	TEST_ASSERT( WgTextPropManager::GetRefCnt( text1.getDefaultProperties().GetHandle() ) == 2+1 );

	// Set a default color, should increase no of properties...

	text1.setColor( WgColor(255,0,0) );
	TEST_ASSERT( WgTextPropManager::GetNbProperties() == 2 );

	TEST_ASSERT( WgTextPropManager::GetRefCnt( text1.getDefaultProperties().GetHandle() ) == 1 );
	TEST_ASSERT( WgTextPropManager::GetRefCnt( text2.getDefaultProperties().GetHandle() ) == 1+1 );

	// Change color, shouldn't affect no of properties (one removed and one added)...

	text1.setColor( WgColor(254,0,0) );
	TEST_ASSERT( WgTextPropManager::GetNbProperties() == 2 );

	TEST_ASSERT( WgTextPropManager::GetRefCnt( text1.getDefaultProperties().GetHandle() ) == 1 );
	TEST_ASSERT( WgTextPropManager::GetRefCnt( text2.getDefaultProperties().GetHandle() ) == 1+1 );

	// Clear color, should decrease no of properties...

	text1.clearColor();
	TEST_ASSERT( WgTextPropManager::GetNbProperties() == 1 );

	TEST_ASSERT( WgTextPropManager::GetRefCnt( text1.getDefaultProperties().GetHandle() ) == 2+1 );

	// Set same default color for both text objects, should create one new property...

	text1.setColor( WgColor(10,0,1) );
	text2.setColor( WgColor(10,0,1) );
	TEST_ASSERT( WgTextPropManager::GetNbProperties() == 2 );

	TEST_ASSERT( WgTextPropManager::GetRefCnt( text1.getDefaultProperties().GetHandle() ) == 2 );

	// Clear color for one, shouldn't remove any property...

	text1.clearColor();
	TEST_ASSERT( WgTextPropManager::GetNbProperties() == 2 );

	// Clear color for the last one should remove the property...

	text2.clearColor();
	TEST_ASSERT( WgTextPropManager::GetNbProperties() == 1 );

	// Set color of some characters...

	text1.setColor( WgColor( 19,19,19 ), WgTextBlock( 0,2,0,4 ) );
	text1.setColor( WgColor( 18,18,18 ), WgTextBlock( 0,5,0,7 ) );
	text2.setColor( WgColor( 19,19,19 ), WgTextBlock( 0,0,0,4 ) );

	TEST_ASSERT( WgTextPropManager::GetNbProperties() == 3 );

	// Clear first color, except for one char, shouldn't affect nb properties

	text1.clearColor( WgTextBlock(0,2,0,3) );
	TEST_ASSERT( WgTextPropManager::GetNbProperties() == 3 );

	// Clear color of text2, shouldn't affect nb properties

	text2.clearColor();

	// Clear the last char with first color, should decrease nb properties....

	text1.clearColor( WgTextBlock(0,3,0,4) );
	TEST_ASSERT( WgTextPropManager::GetNbProperties() == 2 );

	// Set text1 to new text, should decrease nb properties...

	text1.setText( "Yet another test text" );
	TEST_ASSERT( WgTextPropManager::GetNbProperties() == 1 );

	// Underline "another test" and set "Yet another" to selected.

	text1.setUnderlined( WgTextBlock(0,4,0,16) );
	text1.setSelected( WgTextBlock(0,0,0,11) );
	TEST_ASSERT( WgTextPropManager::GetNbProperties() == 4 );

	// Set "t ano" of Yet another, to a different color, should increase nb properties with two...

	text1.setColor( WgColor(255,0,0), WgTextBlock( 0,2,0,7 ) );
	TEST_ASSERT( WgTextPropManager::GetNbProperties() == 6 );

	// Remove selected from part not combined with other attribute, should decrease nb properties...

	text1.clearSelected( WgTextBlock( 0,0,0,2 ) );
	TEST_ASSERT( WgTextPropManager::GetNbProperties() == 5 );

	// Set all text content underlined, directly on cells, should decrease nb properties with two...

	text1.setUnderlined( WgTextBlock( 0,0,1000,1000 ) );
	TEST_ASSERT( WgTextPropManager::GetNbProperties() == 4 );

	// Copy text to a new one, shouldn't affect nb properties...

	text2.setText( &text1 );
	TEST_ASSERT( WgTextPropManager::GetNbProperties() == 4 );

	// Set text object 2 as underlined default, but don't clear cells, should't affect nb properties since there
	// are cells with just the underlined property...

	text2.setUnderlined(false);
	TEST_ASSERT( WgTextPropManager::GetNbProperties() == 4 );

	// Set text object 2 as underlined default with clearing of cells, should increase nb properties with two...

	text2.setUnderlined();
	TEST_ASSERT( WgTextPropManager::GetNbProperties() == 6 );

	// Clear text in object 1, should decrease nb properties with 2...

	text1.clear();
	TEST_ASSERT( WgTextPropManager::GetNbProperties() == 4 );

	// Deleting all colored characters from object 2 except for one, shouldn't affect nb properties...

	text2.removeChar(0, 2);
	text2.removeChar(0, 2);
	text2.removeChar(0, 2);
	text2.removeChar(0, 2);
	TEST_ASSERT( WgTextPropManager::GetNbProperties() == 4 );

	// Remove 5 characters from object 2, should remove all characters with properties
	// and thus decrease nb properties with two...
	// (only emtpy and default for text2 left)

	text2.removeText( 0,2,5 );
	TEST_ASSERT( WgTextPropManager::GetNbProperties() == 2 );
	TEST_ASSERT( text2.nbChars() == 12 );

	// Clear text object 2, should keep the default properties so shouldn't affect nb properties...

	text2.clear();
	TEST_ASSERT( WgTextPropManager::GetNbProperties() == 2 );


	return true;
}


bool TextTest::PropertiesOnWgCharArrays()
{
	// Check so everything is in order to start the test

	TEST_ASSERT( WgTextPropManager::GetNbProperties() == 1 );
	TEST_ASSERT( WgTextPropManager::GetPropBufferSize() == 1 );

/*
	Tilldela sig själv, både WgChar WgTextPropPtr...

	Testa direkt bearbetning av WgChar-buffer.

*/

	return true;
}
