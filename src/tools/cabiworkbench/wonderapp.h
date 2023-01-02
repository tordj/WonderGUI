
#pragma once

#include "baseplate.h"

#include <wondergui.h>
#include <wg_freetypefont.h>

#include <initializer_list>
#include <string>

#include <wg_cabi_base.h>
#include <wg_cabi_root_interface.h>
#include <wg_cabi_capsule.h>
#include <wg_cabi_root.h>

using namespace wg;

class MyApp : public WonderApp
{
public:
	Size	startWindowSize();

	bool		init(Visitor* pVisitor) override;
	bool		update() override;
	void		exit() override;


private:

	bool			_setupGUI(Visitor* pVisitor);
	bool			_loadSkins(Visitor* pVisitor);

	void			_setupCABIClient(wg_c_callCollection* pBaseInterface, wg_cabi_root_outcalls* pRootInterface);

	WonderApp::Visitor * m_pAppVisitor = nullptr;
	WonderApp::Window_p m_pWindow = nullptr;

	CABICapsule_p	m_pCABICapsule;

	CABIRoot_p		m_pCABIRoot;

	Skin_p			m_pPlateSkin;
	Skin_p			m_pButtonSkin;
	Skin_p			m_pToggleButtonSkin;
	Skin_p			m_pCheckBoxSkin;
	Skin_p			m_pSectionSkin;

	PackLayout_p	m_pLayout;
	
	TextStyle_p		m_pTextStyle;
	TextStyle_p		m_pLabelStyle;

	TextLayout_p	m_pTextLayoutCentered;
};
