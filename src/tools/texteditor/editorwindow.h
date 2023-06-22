
#pragma once

#include <wonderapp.h>

#include <wondergui.h>
#include <wg_freetypefont.h>

#include <initializer_list>
#include <string>

using namespace wg;


class EditorWindow;
typedef StrongPtr<EditorWindow>		EditorWindow_p;

class MyApp;

class EditorWindow : public Object
{
public:

	static EditorWindow_p create(Window_p pWindow, MyApp* pApp) { return EditorWindow_p(new EditorWindow(pWindow, pApp)); }

	void	setContent(const char* pContent);

protected:

	EditorWindow(Window_p pWindow, MyApp* pApp);
	~EditorWindow();

	bool			_setupGUI();
	Widget_p 		_createTopBar();
	void			_selectAndLoadFile();
	bool			_saveFile();

	MyApp*			m_pApp;
	Window_p	    m_pWindow;

	RootPanel_p		m_pRootPanel;
	TextEditor_p	m_pEditor;

	std::string		path;
};

