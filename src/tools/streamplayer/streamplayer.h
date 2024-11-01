
#pragma once

#include <wonderapp.h>

#include <wondergui.h>
#include <wg_freetypefont.h>

#include <wg_softsurface.h>

#include <initializer_list>
#include <string>

#include <wondergfx_c.h>

using namespace wg;

class MyApp : public WonderApp
{
public:

	bool		init(Visitor* pVisitor) override;
	bool		update() override;
	void		exit() override;


private:

	bool			_setupGUI(Visitor* pVisitor);
	bool			_loadStream(std::string path);

	bool			_setupStreamPlaying();


	WonderApp::Visitor * m_pAppVisitor = nullptr;
	Window_p			m_pWindow = nullptr;
	
	TextStyle_p		m_pTextStyle;
	TextStyle_p		m_pLabelStyle;

	TextLayout_p	m_pTextLayoutCentered;

	SurfaceDisplay_p	m_pDisplay;

	Blob_p			m_pStreamBlob;


	wg_obj 			m_wrapper = nullptr;
	wg_obj 			m_player = nullptr;
	wg_obj 			m_pump = nullptr;
	wg_obj			m_streamGfxDevice = nullptr;

	wg_obj			m_streamFrontCanvas = nullptr;
	wg_obj			m_streamBackCanvas = nullptr;

	wg_patches		m_streamFrontPatches = nullptr;
	wg_patches		m_streamBackPatches = nullptr;
};
