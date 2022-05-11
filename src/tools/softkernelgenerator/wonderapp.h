
#pragma once

#include "baseplate.h"
#include "kerneldb.h"

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

	void		tintModeToggled(wg::Msg* pMsg);
	void		blendModeToggled(wg::Msg* pMsg);
	void		sourceFormatToggled(wg::Msg* pMsg);
	void		destFormatToggled(wg::Msg* pMsg);

	void		generateSource();
	void		clear();
	void		reset();
	void		load();
	void		save();


private:

	bool			_setupGUI(Visitor* pVisitor);
	bool			_loadSkins(Visitor* pVisitor);
	wg::Widget_p	_buildButtonRow();
	wg::Widget_p	_buildList();

	wg::Widget_p	_buildGlobalSettingsSection();
	wg::Widget_p	_buildOptimizedBlitsSection();
	wg::Widget_p	_buildListSummarySection();
	wg::Widget_p	_buildExportSection();

	wg::Widget_p	_buildLabeledList(int nColumns, std::initializer_list < std::pair<std::string, int>> list);

	void			_refreshSummary();
	void			_refreshList();


	KernelDB *	m_pDB = nullptr;

	wg::Skin_p			m_pPlateSkin;
	wg::Skin_p			m_pButtonSkin;
	wg::Skin_p			m_pToggleButtonSkin;
	wg::Skin_p			m_pCheckBoxSkin;

	wg::TextStyle_p		m_pTextStyle;
	wg::TextStyle_p		m_pLabelStyle;

	wg::ScrollPanel_p	m_pWindow;
	wg::PackPanel_p		m_pList;
};