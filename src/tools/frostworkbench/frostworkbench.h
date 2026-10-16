#pragma once

#include <wonderapp.h>
#include <wondergui.h>

using namespace wg;

class MyApp : public WonderApp
{
public:
	bool		init(wapp::API* pAPI) override;
	bool		update() override;
	void		exit() override;

	void		closeWindow(wapp::Window* pWindow) override;

private:

	bool		setupGUI(wapp::API * pAPI);

	Surface_p	createRoundedRect(int radius, int thickness, HiColor color);
	void		makeDraggable(Widget * pPane);

	wapp::Window_p	m_pWindow;
	FrostOverlay_p	m_pOverlay;
	FlexPanel_p		m_pBackdrop;
	Widget_p		m_pBouncer;

	Coord			m_bouncerSpeed = { 3, 2 };
};
