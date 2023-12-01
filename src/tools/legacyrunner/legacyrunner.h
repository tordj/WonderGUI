
#pragma once

#include <wonderapp.h>
#include <wondergui.h>

#include <wglegacygui.h>

#include <initializer_list>
#include <string>

class MyApp : public WonderApp
{
public:
	bool		init(Visitor* pVisitor) override;
	bool		update() override;
	void		exit() override;




private:

	bool			_setupGUI(Visitor* pVisitor);
	bool			_loadSkins(Visitor* pVisitor);

	void			_setupLegacyKeyMap();
	WgWidget *		_buildLegacyGUI();


	Window_p			m_pWindow;

	wg::Size			m_dragStartSize;

	wg::Skin_p			m_pPlateSkin;
	wg::Skin_p			m_pButtonSkin;
	wg::Skin_p			m_pTightButtonSkin;
	wg::Skin_p			m_pToggleButtonSkin;
	wg::Skin_p			m_pCheckBoxSkin;
	wg::Skin_p			m_pSectionSkin;

	wg::PackLayout_p	m_pLayout;
	wg::PackPanel_p		m_pPackPanel;
	
	wg::TextStyle_p		m_pTextStyle;
	wg::TextStyle_p		m_pLabelStyle;

	wg::TextLayout_p	m_pTextLayoutCentered;

	wg::FlexPanel_p		m_pBasePanel;
};


