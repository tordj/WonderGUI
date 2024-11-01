
#pragma once

#include <wonderapp.h>

#include <wondergui.h>
#include <wg_freetypefont.h>

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

	bool			_setupGUI(Visitor* pVisitor);

	WonderApp::Visitor * m_pAppVisitor = nullptr;
	Window_p			m_pWindow = nullptr;
	
	TextStyle_p		m_pTextStyle;
	TextStyle_p		m_pLabelStyle;

	TextLayout_p	m_pTextLayoutCentered;
};
