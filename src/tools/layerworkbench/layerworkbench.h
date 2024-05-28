
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

	Window_p	m_pWindow;
	
};


