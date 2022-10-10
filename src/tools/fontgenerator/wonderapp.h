
#pragma once

#include "baseplate.h"

#include <wondergui.h>

#include <initializer_list>
#include <string>

class MyApp : public WonderApp
{
public:
	wg::Size	startWindowSize();

	bool		init(Visitor* pVisitor);
	bool		update();
	void		exit();

	wg::Widget_p 	createInputPanel();
	wg::Widget_p	createCharsPanel();
	wg::Widget_p	createOutputPanel();

	void		load();
	void		save();




private:

	bool			_setupGUI(Visitor* pVisitor);
	bool			_loadSkins(Visitor* pVisitor);



	wg::Skin_p			m_pPlateSkin;
	wg::Skin_p			m_pButtonSkin;
	wg::Skin_p			m_pToggleButtonSkin;
	wg::Skin_p			m_pCheckBoxSkin;
	wg::Skin_p			m_pSectionSkin;

	wg::PackLayout_p	m_pLayout;
	
	wg::TextStyle_p		m_pTextStyle;
	wg::TextStyle_p		m_pLabelStyle;

	wg::TextLayout_p	m_pTextLayoutCentered;

	wg::ScrollPanel_p	m_pWindow;
	wg::PackPanel_p		m_pList;
};
