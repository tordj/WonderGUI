
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

	void		tintModeToggled(wg::Msg* pMsg);
	void		blendModeToggled(wg::Msg* pMsg);
	void		sourceFormatToggled(wg::Msg* pMsg);
	void		destFormatToggled(wg::Msg* pMsg);

private:

	bool			_setupGUI(Visitor* pVisitor);
	wg::Widget_p	_buildList();

	KernelDB *	m_pDB = nullptr;

	wg::Skin_p			m_pPlateSkin;
	wg::Skin_p			m_pButtonSkin;
	wg::Skin_p			m_pToggleButtonSkin;

	wg::TextStyle_p		m_pTextStyle;
	wg::TextStyle_p		m_pLabelStyle;


};