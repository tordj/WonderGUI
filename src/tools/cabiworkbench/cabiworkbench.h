
#pragma once

#include <wonderapp.h>

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
	bool		init(Visitor* pVisitor) override;
	bool		update() override;
	void		exit() override;


private:

	typedef int (*initClientFunc)( wg_c_callCollection * pBaseInterface, wg_cabi_root_outcalls * pRootInterface, void * pHostBridge, wg_obj hGfxDevice, wg_obj hSurfaceFactory );
	typedef int (*updateClientFunc)(void);
	typedef void (*exitClientFunc)(void);

	typedef void (*mapInputKeyFunc)(uint32_t native_keycode, wg_key translated_keycode);
	typedef void (*mapInputCommandFunc)(uint32_t native_keycode, wg_modkeys modkeys, wg_editCommand command);

	
	bool			_setupGUI(Visitor* pVisitor);
	bool			_loadSkins(Visitor* pVisitor);


	WonderApp::Visitor * m_pAppVisitor = nullptr;
	WonderApp::Window_p m_pWindow = nullptr;
	WonderApp::LibId	m_libId = 0;


	CABICapsule_p	m_pCABICapsule;

	CABIRoot_p		m_pCABIRoot;

	initClientFunc	m_pInitClient;
	updateClientFunc	m_pUpdateClient;
	exitClientFunc	m_pExitClient;

	
	
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
