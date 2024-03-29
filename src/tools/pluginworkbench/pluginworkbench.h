
#pragma once

#include <wonderapp.h>

#include <wondergui.h>
#include <wg_freetypefont.h>

#include <initializer_list>
#include <string>

#include <wg_pluginbase.h>
#include <wg_plugininterface.h>
#include <wg_plugincapsule.h>
#include <wg_pluginroot.h>

using namespace wg;

class MyApp : public WonderApp
{
public:
	bool		init(Visitor* pVisitor) override;
	bool		update() override;
	void		exit() override;


private:

	typedef int (*initClientFunc)( wg_plugin_interface * pInterface, void * pHostBridge, wg_obj hPluginCapsule, wg_obj hGfxDevice, wg_obj hSurfaceFactory, wg_obj hEdgemapFactory );
	typedef int (*updateClientFunc)(void);
	typedef void (*exitClientFunc)(void);

	typedef void (*mapInputKeyFunc)(uint32_t native_keycode, wg_key translated_keycode);
	typedef void (*mapInputCommandFunc)(uint32_t native_keycode, wg_modkeys modkeys, wg_editCommand command);

	
	bool			_setupGUI(Visitor* pVisitor);
	bool			_loadSkins(Visitor* pVisitor);


	WonderApp::Visitor * m_pAppVisitor = nullptr;
	Window_p			m_pWindow = nullptr;
	WonderApp::LibId	m_libId = 0;


	PluginCapsule_p		m_pPluginCapsule;


	initClientFunc		m_pInitClient;
	updateClientFunc	m_pUpdateClient;
	exitClientFunc		m_pExitClient;

	
	
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
