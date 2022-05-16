
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

	void		customBlitTypeToggled(int index, wg::Msg* pMsg);
	void		customBlitTintModeToggled(int index, wg::Msg* pMsg);
	void		customBlitBlendModeToggled(int index, wg::Msg* pMsg);
	void		customBlitSourceFormatToggled(int index, wg::Msg* pMsg);
	void		customBlitDestFormatToggled(int index, wg::Msg* pMsg);


	void		eraseCustomBlitEntry(int index);

	void		addOptimizedBlitEntry();
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
	wg::PackPanel_p	_buildToggleButtonRow(std::string title, std::vector<KernelDB::BlitType> blitTypes, const bool selected[], std::function<void(wg::Msg*)> pressCallback);
	wg::PackPanel_p	_buildToggleButtonRow(std::string title, std::vector<wg::BlendMode> blendModes, const bool selected[], std::function<void(wg::Msg*)> pressCallback);
	wg::PackPanel_p	_buildToggleButtonRow(std::string title, std::vector<wg::TintMode> blendModes, const bool selected[], std::function<void(wg::Msg*)> pressCallback);
	wg::PackPanel_p	_buildToggleButtonRow(std::string title, std::vector<wg::PixelFormat> blendModes, const bool selected[], std::function<void(wg::Msg*)> pressCallback);
	wg::Widget_p	_buildHeaderWithCloseButton(std::string title, std::function<void(wg::Msg*)> pressCallback);

	void			_refreshSummary();
	void			_refreshList();


	KernelDB *	m_pDB = nullptr;

	wg::Skin_p			m_pPlateSkin;
	wg::Skin_p			m_pButtonSkin;
	wg::Skin_p			m_pToggleButtonSkin;
	wg::Skin_p			m_pCheckBoxSkin;
	wg::Skin_p			m_pSectionSkin;

	wg::TextStyle_p		m_pTextStyle;
	wg::TextStyle_p		m_pLabelStyle;

	wg::TextLayout_p	m_pTextLayoutCentered;

	wg::ScrollPanel_p	m_pWindow;
	wg::PackPanel_p		m_pList;
};