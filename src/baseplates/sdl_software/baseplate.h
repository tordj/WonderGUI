#include <wg_object.h>
#include <wg_blob.h>
#include <wg_surface.h>
#include <wg_rootpanel.h>

#pragma once


class WonderApp;
typedef	wg::StrongPtr<WonderApp>	WonderApp_p;
typedef	wg::WeakPtr<WonderApp>		WonderApp_wp;


class WonderApp : public wg::Object
{
public:

	enum class IconType
	{
		Info,
		Warning,
		Error,
		Question
	};
	
	enum class DialogType
	{
		Ok,
		OkCancel,
		YesNo,
		YesNoCancel		
	};
	
	enum class DialogButton
	{
		Undefined,
		Cancel,
		No,
		Yes,
		Ok
	};
	
	
	static WonderApp_p create();

	class Visitor
	{
	public:
		virtual wg::RootPanel_p	rootPanel() = 0;
		virtual int64_t			time() = 0;					// Time in millisec since any arbitrary time before call to init().

		virtual wg::Blob_p		loadBlob(const std::string& path) = 0;
		virtual wg::Surface_p	loadSurface(const std::string& path, wg::SurfaceFactory* pFactory = nullptr,
											const wg::Surface::Blueprint& blueprint = wg::Surface::Blueprint() ) = 0;
		
		virtual bool			notifyPopup(char const * title, char const * message, IconType iconType) = 0;
		
		virtual DialogButton	messageBox(char const * title, char const * message, DialogType dialogType,
										   IconType iconType, DialogButton defaultButton = DialogButton::Undefined ) = 0;

		virtual char *			inputBox(char const * title, char const * message, char const * defaultInput ) = 0;

		virtual char *			saveFileDialog( char const * title, char const * defaultPathAndFile,
											    int nbFilterPatterns,	char const * const * fFilterPatterns,
											    char const * singleFilterDescription ) = 0;
		
		virtual char * 			openFileDialog(	char const * title, char const * defaultPathAndFile,
											    int nbFilterPatterns, char const * const * filterPatterns,
											    char const * singleFilterDescription, bool bMultiSelec ) = 0;

		virtual char *			selectFolderDialog( char const * title, char const * defaultPath) = 0;
	};

	virtual wg::Size	startWindowSize() = 0;

	virtual bool	init(Visitor* pVisitor) = 0;
	virtual bool	update() = 0;
	virtual void	exit() = 0;
	
protected:

	~WonderApp() {}
	
};
