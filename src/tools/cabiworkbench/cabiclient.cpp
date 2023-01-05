
#include <wondergui.h>
#include <wg_cabi.h>
#include <wg_freetypefont.h>

#include <string>
#include <fstream>



#include <dlfcn.h>

using namespace wg;
using namespace std;


extern "C" {

	int		mySimpleCall(void);
	int		init( wg_c_callCollection * pBaseInterface, wg_cabi_root_outcalls * pRootInterface );
	int		update(void);
	void	exitX(void);
}

int mySimpleCall(void)
{
	return 123;
}





CABIRoot_p g_pCABIRoot;

//____ init() _________________________________________________________________

int init( wg_c_callCollection * pBaseInterface, wg_cabi_root_outcalls * pRootInterface )
{
	Base::init(nullptr);
	
	CABI::init(pBaseInterface);

	auto pCABIRoot = CABIRoot::create(pRootInterface);

	auto pMainPanel = PackPanel::create();
	pMainPanel->setAxis(Axis::Y);
	pMainPanel->setSkin(ColorSkin::create({ .color = Color::DarkGray, .padding = 4 }));

	auto pLayout = PackLayout::create({
		.expandFactor = PackLayout::Factor::Weight
		});

	pMainPanel->setLayout(pLayout);

	auto pFiller1 = Filler::create({ .skin = ColorSkin::create({.color = Color::Red }) });
	auto pFiller2 = Filler::create({ .skin = ColorSkin::create({.color = Color::Green }) });
	auto pFiller3 = Filler::create({ .skin = ColorSkin::create({.color = Color::Blue }) });

	pMainPanel->slots << pFiller1;
	pMainPanel->slots << pFiller2;
	pMainPanel->slots << pFiller3;

	pCABIRoot->slot = pMainPanel;

	g_pCABIRoot = pCABIRoot;
	
	
	
	return 1;
}

//____ update() _______________________________________________________________

int update(void)
{
	return 1;
}

//____ exit() _________________________________________________________________

void exitX(void)
{
	Base::exit();
}

