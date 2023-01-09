
#include <wondergui.h>
#include <wg_cabi.h>
#include <wg_freetypefont.h>

#include <string>
#include <fstream>

using namespace wg;
using namespace std;


#ifdef WIN32
#define	DLLEXPORTPREFIX __declspec(dllexport)
#else
#define	DLLEXPORTPREFIX
#endif

extern "C" {
	DLLEXPORTPREFIX int		init( wg_c_callCollection * pBaseInterface, wg_cabi_root_outcalls * pRootInterface );
	DLLEXPORTPREFIX int		update(void);
	DLLEXPORTPREFIX void	exitX(void);
}




CABIRoot_p g_pCABIRoot;

//____ init() _________________________________________________________________

DLLEXPORTPREFIX int init( wg_c_callCollection * pBaseInterface, wg_cabi_root_outcalls * pRootInterface )
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

	auto pSkin1 = ColorSkin::create({ .states = { {State::Normal, Color::Red}, {State::Hovered, Color::Pink}, {State::Pressed, Color::DarkRed} }  });
	auto pSkin2 = ColorSkin::create({ .states = { {State::Normal, Color::Green}, {State::Hovered, Color::LightGreen}, {State::Pressed, Color::DarkGreen} }  });
	auto pSkin3 = ColorSkin::create({ .states = { {State::Normal, Color::Blue}, {State::Hovered, Color::LightBlue}, {State::Pressed, Color::DarkBlue} }  });

	
	auto pFiller1 = Filler::create({ .skin = pSkin1 });
	auto pFiller2 = Filler::create({ .skin = pSkin2 });
	auto pFiller3 = Filler::create({ .skin = pSkin3 });

	pMainPanel->slots << pFiller1;
	pMainPanel->slots << pFiller2;
	pMainPanel->slots << pFiller3;

	pCABIRoot->slot = pMainPanel;

	g_pCABIRoot = pCABIRoot;
	
	
	
	return 1;
}

//____ update() _______________________________________________________________

DLLEXPORTPREFIX int update(void)
{
	return 1;
}

//____ exit() _________________________________________________________________

DLLEXPORTPREFIX void exitX(void)
{
	Base::exit();
}

