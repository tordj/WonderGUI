
#include "layerworkbench.h"

#include <wondergui.h>

#include <wg_freetypefont.h>
#include <string>
#include <fstream>

using namespace wg;
using namespace std;

//____ create() _______________________________________________________________

WonderApp_p WonderApp::create()
{
	return new MyApp();
}

//____ init() _________________________________________________________________

bool MyApp::init(Visitor* pVisitor)
{
	if (!setupGUI(pVisitor))
	{
		printf("ERROR: Failed to setup GUI!\n");
		return false;
	}

	
	
	
	return true;
}

//____ update() _______________________________________________________________

bool MyApp::update()
{
	return true;
}

//____ exit() _________________________________________________________________

void MyApp::exit()
{
}

//____ setupGUI() _____________________________________________________________

bool MyApp::setupGUI(Visitor* pVisitor)
{
	// Setup window
	
	auto pWindow = pVisitor->createWindow( {
		.title = "Layer Workbench"
	});
	
	m_pWindow = pWindow;

	auto pRoot = m_pWindow->rootPanel();
	
	auto pLayers = CanvasLayers::create({
		.baseLayer = 2,
		.layers = {
			{ .format = PixelFormat::Alpha_8,
				.blendFunc = [](GfxDevice* pDevice)
				{
					pDevice->setBlendMode(BlendMode::Subtract);
					pDevice->blit({0,0});
					pDevice->setBlendMode(BlendMode::Blend);
				},
				.clearFunc = [](GfxDevice* pDevice)
				{
					pDevice->setBlendMode(BlendMode::Replace);
					pDevice->fill(HiColor::Transparent);
					pDevice->setBlendMode(BlendMode::Blend);
				}
			},
			{ .format = PixelFormat::BGRA_8 },
			{ .format = PixelFormat::BGRA_8 }
		}
	});
	
	pRoot->setCanvasLayers(pLayers);
	
	pRoot->setSkin( ColorSkin::create({ .color = Color::Grey, .layer = 0} ));
	
	// Create skins
	
	auto pShadowSkin = ColorSkin::create( {
		.blendMode = BlendMode::Max,
		.color = HiColor{0,0,0,256},
		.layer = 1,
		.margin = { 20,0,0,20 },
		.overflow = {0,20,20,0}
	});
	
	auto pGreenBox = BoxSkin::create( {
		.color = Color::Green,
		.outlineColor = Color::Black,
		.outlineThickness = 2
	});
	
	auto pBoxWithShadow = DoubleSkin::create({
		.skinInSkin = false,
		.skins = { pGreenBox, pShadowSkin }
	});
	
	// Setup layout
	
	auto pBaseFlex = FlexPanel::create();
	pRoot->slot = pBaseFlex;
	
	pBaseFlex->slots << createMovableBox(pBoxWithShadow,pBaseFlex);
	pBaseFlex->slots << createMovableBox(pBoxWithShadow,pBaseFlex);
	pBaseFlex->slots << createMovableBox(pBoxWithShadow,pBaseFlex);

	return true;
}

//____ createMovableBox() _______________________________________________

Widget_p MyApp::createMovableBox( Skin * pSkin, FlexPanel * pParent )
{
	auto pBox = Filler::create({
		.defaultSize = {64,64},
		.skin = pSkin
	});
	
	Base::msgRouter()->addRoute(pBox, MsgType::MouseDrag, [pBox,pParent](Msg * _pMsg)
	{
		auto pMsg = static_cast<MouseDragMsg*>(_pMsg);
		
		auto it = pParent->slots.find(pBox);
		
		it->move(pMsg->draggedNow());
	});
	
	return pBox;
}
