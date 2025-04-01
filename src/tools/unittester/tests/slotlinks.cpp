#include <string.h>


#include "slotlinks.h"

#include <wondergui.h>
#include <wondergfxstream.h>

#include <wg_string.h>

using namespace wg;

SlotLinksTest::SlotLinksTest()
{

	ADD_TEST(packPanelSlotLinksTest);

}

SlotLinksTest::~SlotLinksTest()
{
}


bool SlotLinksTest::init(std::ostream& output)
{
	return true;
}

//____ packPanelSlotLinksTest() _______________________________________________

bool SlotLinksTest::packPanelSlotLinksTest(std::ostream& output)
{
	auto pPanel = PackPanel::create();

	for( int i = 0 ; i < 10 ; i++ )
		pPanel->slots << Filler::create();

	auto link0 = pPanel->slots.makeLink(pPanel->slots.begin());
	auto pWidget0 = pPanel->slots[0].widget();

	auto link3 = pPanel->slots.makeLink(3);
	auto pWidget3 = pPanel->slots[3].widget();

	auto link8 = pPanel->slots.makeLink(8);
	auto pWidget8 = pPanel->slots[8].widget();


	TEST_ASSERT(link0->widget() == pWidget0);
	TEST_ASSERT(link3->widget() == pWidget3);
	TEST_ASSERT(link8->widget() == pWidget8);

	pPanel->slots.insert(5, Filler::create() );

	TEST_ASSERT(link0->widget() == pWidget0);
	TEST_ASSERT(link3->widget() == pWidget3);
	TEST_ASSERT(link8->widget() == pWidget8);

	for( int i = 0 ; i < 300 ; i++ )
		pPanel->slots.insert(1, Filler::create() );

	TEST_ASSERT(link0->widget() == pWidget0);
	TEST_ASSERT(link3->widget() == pWidget3);
	TEST_ASSERT(link8->widget() == pWidget8);

	pPanel->slots.erase(2, 200);

	TEST_ASSERT(link0->widget() == pWidget0);
	TEST_ASSERT(link3->widget() == pWidget3);
	TEST_ASSERT(link8->widget() == pWidget8);

	TEST_ASSERT(link0.index() == 0);
	TEST_ASSERT(link0 == true);

	pPanel->slots.erase(0,1);
	TEST_ASSERT(link0.index() == -1);
	TEST_ASSERT(link0.ptr() == nullptr);
	TEST_ASSERT(link0 == false);

	link0 = link3;

	TEST_ASSERT(link0.index() == link3.index() );
	TEST_ASSERT(link0->widget() == pWidget3 );
	TEST_ASSERT(link0 == true);

	PackPanelSlot::Link linkX = pPanel->slots.makeLink(pPanel->slots.begin());

	pPanel = nullptr;

	TEST_ASSERT(link0 == false );
	TEST_ASSERT(link3 == false );
	TEST_ASSERT(link8 == false );



	return true;
}

