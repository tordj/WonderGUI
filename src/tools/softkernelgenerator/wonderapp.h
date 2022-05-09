
#pragma once

#include "baseplate.h"
#include "kerneldb.h"

class MyApp : public WonderApp
{
public:
	wg::Size	startWindowSize();

	bool		init(Visitor* pVisitor);
	bool		update();
	void		exit();

private:

	bool			_setupGUI(Visitor* pVisitor);
	wg::Widget_p	_buildList();

	KernelDB *	m_pDB = nullptr;

	wg::Skin_p			m_pPlateSkin;
	wg::Skin_p			m_pButtonSkin;
	wg::Skin_p			m_pToggleButtonSkin;

};