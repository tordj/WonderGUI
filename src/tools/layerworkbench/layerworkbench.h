
#pragma once

#include <wonderapp.h>
#include <wondergui.h>

#include <initializer_list>
#include <string>

using namespace wg;

class MyApp : public WonderApp
{
public:
	bool		init(Visitor* pVisitor) override;
	bool		update() override;
	void		exit() override;

private:
	
	bool		setupGUI(Visitor * pVisitor);
	
	Widget_p	createMovableBox( Skin * pSkin, FlexPanel * pParent );
	Widget_p	makeMovable(const Widget_p& pWidget, FlexPanel* pParent);


	Window_p	m_pWindow;
	
};


